#include <memory>
#include "Enemy.h"
#include "../Model.h"
#include "../Player/Player.h"
#include "../Bullet/BulletManager.h"
#include "../../Geometry/Vector3.h"
#include "../../Sound/SoundManager.h"
#include "../../Effect/EffectManager.h"
#include "../../MyLib/MyLib.h"
#include "DxLib.h"

namespace
{
	// 初期座標(当たり判定含む)
	const Vector3 kFirstPos = { 200.0f,80.0f,0.0f };
	// モデルのオフセット
	const Vector3 kModelOffset = { 0.0f, 75.0f,0.0f };
	// モデルのスケール
	const Vector3 kModelScale = { 2.0f,2.0f,2.0f };
	// モデルの向き
	const Vector3 kDefaultDir = { 0.0f, 0.0f, -1.0f };

	// エフェクトが出るオフセット
	const Vector3 kEffectOffset = { 0.0f,300.0f,0.0f };

	// 当たり判定の半径
	constexpr float kDefaultRadius = 80.0f;

	// プレイヤーを検知する半径
	constexpr float kDetectRadius = 1800.0f;

	// 向きの変更速度
	constexpr float kLerpSpeed = 0.05f;

	// 弾を撃つときのSEの音量
	constexpr float kShootSEVolume = 100.0f;

	// 検知から攻撃までの時間
	constexpr int kAttackStartTime = 120;

	// 検知を終えるまでの時間
	constexpr int kDetectEndTime = 60;

	// 見失う時間
	constexpr int kLostTime = 120;

	// 敵を倒した際のスコア
	constexpr int kKillScore = 100;

	// アニメーション番号
	constexpr int kIdleAnimNum = 5; // 待機
	constexpr int kDetectAnimNum = 1; // 発見
	constexpr int kAttackAnimNum = 0; // 攻撃
	constexpr int kLostAnimNum = 7; // 見失い
	constexpr int kDamageAnimNum = 3; // ダメージ

	// アニメーションのブレンド時間
	constexpr int kAnimBlendTime = 20;
}

Enemy::Enemy() : 
	EnemyBase(ObjectTag::Enemy, MyLib::ColliderBase::ColliderType::Sphere),
	m_dir{},
	m_detectRadius(0.0f),
	m_detectColor(0),
	m_attackCount(0),
	m_detectEndCount(0),
	m_lostCount(0),
	m_update(&Enemy::IdleUpdate)
{
	// 球の当たり判定を取得
	auto collider = dynamic_pointer_cast<MyLib::SphereCollider>(m_pColliderData);
	// 球の当たり判定の半径を設定
	collider->m_radius = kDefaultRadius;
}

Enemy::~Enemy()
{
}

void Enemy::Init()
{
	// Collidableの初期化処理を呼ぶ
	Collidable::Init();

	m_pModel = std::make_shared<Model>();
	m_pModel->Init();

	// 初期位置を設定
	m_rigidbody.SetPos(kFirstPos);

	// プレイヤーを検知する半径を設定
	m_detectRadius = kDetectRadius;
	m_detectColor = 0xff99ff;

	// 敵の向きをデフォルト状態にする
	m_dir = kDefaultDir;

	// 初期状態を待機状態にする
	m_update = &Enemy::IdleUpdate;

	// サウンドを登録
	auto& soundManager = SoundManager::GetInstance();
	soundManager.LoadSoundClip("Detect", L"Data/Sound/SE/detect.mp3", SoundBus::SE, 1.0f, false); // 発見時のSE
	soundManager.LoadSoundClip("Damage", L"Data/Sound/SE/damage.mp3", SoundBus::SE, 1.0f, false); // ダメージ時のSE
	soundManager.LoadSoundClip("Shoot", L"Data/Sound/SE/shoot.mp3", SoundBus::SE, 1.0f, false); // 攻撃時のSE

	// エフェクトを登録
	auto& effectManager = EffectManager::GetInstance();
	effectManager.LoadEffect(L"detect.efk");
	effectManager.LoadEffect(L"lost.efk");
	effectManager.LoadEffect(L"hit.efk");
	effectManager.LoadEffect(L"enemyDie.efk");
}

void Enemy::End()
{
	// Collidableの終了処理を呼ぶ
	Collidable::End();

	m_pModel->End();

	// 登録したサウンドを開放
	auto& soundManager = SoundManager::GetInstance();
	soundManager.DeleteSoundClip("Detect"); // 発見時のSE
	soundManager.DeleteSoundClip("Damage"); // ダメージ時のSE
	soundManager.DeleteSoundClip("Shoot"); // 攻撃時のSE

	// 登録したエフェクトを開放
	auto& effectManager = EffectManager::GetInstance();
	effectManager.DeleteEffect(L"detect.efk");
	effectManager.DeleteEffect(L"lost.efk");
	effectManager.DeleteEffect(L"hit.efk");
	effectManager.DeleteEffect(L"enemyDie.efk");
}

void Enemy::Update()
{
	// 更新処理を行う
	(this->*m_update)();

	// モデルの更新
	UpdateModel();
}

void Enemy::Draw() const
{
	m_pModel->Draw();
#ifdef _DEBUG
	// デバッグ用の検知範囲を表示
	DrawSphere3D(m_rigidbody.GetPos().ToDxLibVector(), m_detectRadius, 8, m_detectColor, m_detectColor, false);
#endif // _DEBUG
}

void Enemy::OnCollide(std::shared_ptr<Collidable> collider)
{
}

const int Enemy::GetKillScore() const
{
	return kKillScore;
}

void Enemy::SetModel(int handle)
{
	m_pModel->SetModelHandle(MV1DuplicateModel(handle));
	// モデルにアニメーションをセット(Initより前にこれを呼ぶため)
	m_pModel->SetAnimation(kIdleAnimNum, 1.0f, 0, true);
	// モデルのスケールをセット
	m_pModel->SetScale(kModelScale);
}

void Enemy::SetSpawnPos(const Vector3& pos)
{
	m_rigidbody.SetPos(pos);

	// モデルの位置も更新(オフセット分ずらす)
	m_pModel->SetPos(pos - kModelOffset);
}

void Enemy::SetPlayer(std::shared_ptr<Player> player)
{
	m_pPlayer = player;
}

void Enemy::SetBulletManager(std::shared_ptr<BulletManager> manager)
{
	m_pBulletManager = manager;
}

void Enemy::Damage()
{
	// ダメージのSEを再生
	SoundManager::GetInstance().Play("Damage", 1.0f, true);
	// ダメージエフェクトを生成
	EffectManager::GetInstance().CreateEffect(L"hit.efk", m_rigidbody.GetPos());
	// 状態を死亡中のものにする
	m_state = EnemyState::Dying;
	m_update = &Enemy::DamageUpdate;
	m_pModel->SetAnimation(kDamageAnimNum, 0.5f, kAnimBlendTime, false);
}

bool Enemy::IsInDetectRange()
{
	// カメラの範囲外なら見つからなかったとする
	if (!m_isInCamera)
	{
		m_detectColor = 0xff99ff;
		return false;
	}

	// プレイヤーを取得することができたら
	if (auto player = m_pPlayer.lock())
	{
		// プレイヤーの座標を取得
		Vector3 playerPos = player->GetPos();

		// プレイヤーまでの距離を取得
		Vector3 toPlayer = playerPos - m_rigidbody.GetPos();

		// 距離の大きさが検知半径の大きさよりも大きかったら
		if (toPlayer.SqrLength() <= m_detectRadius * m_detectRadius)
		{
			m_detectColor = 0x2211ff;
			return true;
		}
		else
		{
			m_detectColor = 0xff99ff;
			return false;
		}
	}
	return false;
}

void Enemy::Shoot()
{
	// 存在していない時は処理をしない
	if (!IsAlive()) return;
	// バレットマネージャーを取得できたら
	if (auto bManager = m_pBulletManager.lock())
	{
		// 弾発射のSEを再生
		SoundManager::GetInstance().Play("Shoot", kShootSEVolume, true);
		// 弾を生成
		bManager->CreateBullet(m_rigidbody.GetPos(), m_dir, shared_from_this());
	}
}

void Enemy::IdleUpdate()
{

	m_dir = Vector3::LerpVec3(m_dir, kDefaultDir, kLerpSpeed);

	// プレイヤーが検知範囲内に入ったら
	if (IsInDetectRange())
	{
		// 発見した時の処理を行う
		OnDetect();
	}
}

void Enemy::DetectUpdate()
{
	// プレイヤーが取得できるかをチェック
	if (auto player = m_pPlayer.lock())
	{
		// プレイヤーまでの距離を取得
		Vector3 toPlayer = player->GetPos() - m_rigidbody.GetPos();

		// プレイヤーを向くようにする
		m_dir = Vector3::LerpVec3(m_dir, toPlayer.Normalized(), kLerpSpeed);
	}
	
	m_attackCount++;

	if (m_attackCount >= kAttackStartTime)
	{
		m_attackCount = 0;
		m_update = &Enemy::AttackUpdate;
		m_pModel->SetAnimation(kAttackAnimNum, 0.5f, kAnimBlendTime, false);
	}
	else if(!IsInDetectRange())
	{
		// 見失う時間を設定
		m_lostCount = kLostTime;

		m_attackCount = 0;

		// プレイヤー見失い時のエフェクトを生成
		EffectManager::GetInstance().CreateEffect(L"lost.efk", m_rigidbody.GetPos() + kEffectOffset);
		// 更新処理を見失った時のものにする
		m_update = &Enemy::LostUpdate;
		m_pModel->SetAnimation(kLostAnimNum, 0.5f, kAnimBlendTime, true);
	}

}

void Enemy::AttackUpdate()
{
	// プレイヤーが取得できるかをチェック
	if (auto player = m_pPlayer.lock())
	{
		// プレイヤーまでの距離を取得
		Vector3 toPlayer = player->GetPos() - m_rigidbody.GetPos();

		// プレイヤーを向くようにする
		m_dir = Vector3::LerpVec3(m_dir, toPlayer.Normalized(), kLerpSpeed);
	}

	// 攻撃カウンタを増加
	m_attackCount++;

	// 一定時間たったら
	if (m_pModel->GetAnimationEnd())
	{
		// 弾を生成
		Shoot();

		// プレイヤーの検知
		if (IsInDetectRange())
		{
			// 見つかったら検知状態に遷移
			m_attackCount = 0;
			m_update = &Enemy::DetectUpdate;
			m_pModel->SetAnimation(kDetectAnimNum, 0.5f, kAnimBlendTime, true);
		}
		else
		{
			// プレイヤー見失い時のエフェクトを生成
			EffectManager::GetInstance().CreateEffect(L"lost.efk", m_rigidbody.GetPos() + kEffectOffset);
			// 見つからなかったら見失い処理に遷移
			m_lostCount = kLostTime;
			m_update = &Enemy::LostUpdate;
			m_pModel->SetAnimation(kLostAnimNum, 0.5f, kAnimBlendTime, true);
		}
	}
}

void Enemy::LostUpdate()
{
	m_lostCount--;

	if (IsInDetectRange())
	{
		// 発見した時の処理を行う
		OnDetect();
		return;
	}


	if (m_lostCount <= 0)
	{
		m_lostCount = 0;
		m_update = &Enemy::IdleUpdate;
		m_pModel->SetAnimation(kIdleAnimNum, 1.0f, kAnimBlendTime, true);
	}

}

void Enemy::DamageUpdate()
{
	// アニメーションが終わったら完全に死亡した状態にする
	if (m_pModel->GetAnimationEnd())
	{
		// 敵が死んだときのエフェクトを生成
		EffectManager::GetInstance().CreateEffect(L"enemyDie.efk", m_rigidbody.GetPos());
		m_state = EnemyState::Dead;
	}
}

void Enemy::UpdateModel()
{
	// モデルの更新処理を行う
	m_pModel->Update();

	// オフセット分ずらした位置にモデルをセット
	m_pModel->SetPos(m_rigidbody.GetPos() - kModelOffset);

	// モデルの向きをセット
	m_pModel->SetDir(m_dir);
}

void Enemy::OnDetect()
{
	// カメラの範囲外なら処理をしない
	if (!m_isInCamera)return;

	// プレイヤー発見時のエフェクトを生成
	EffectManager::GetInstance().CreateEffect(L"detect.efk", m_rigidbody.GetPos() + kEffectOffset);
	// プレイヤー発見時のSEを再生
	SoundManager::GetInstance().Play("Detect", 1.0f, true);
	// 更新処理を発見時のものにする
	m_update = &Enemy::DetectUpdate;
	m_pModel->SetAnimation(kDetectAnimNum, 0.5f, kAnimBlendTime, true);
}
