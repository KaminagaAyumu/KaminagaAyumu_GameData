#include <memory>
#include "MoveEnemy.h"
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
	constexpr float kDetectRadius = 1000.0f;

	// 向きの変更速度
	constexpr float kLerpSpeed = 0.15f;

	// 弾を撃つときのSEの音量
	constexpr float kShootSEVolume = 100.0f;

	// 敵の動くスピード
	constexpr float kMoveSpeed = 8.0f;

	// プレイヤーに近づく最小距離
	constexpr float kMinDistanceToPlayer = 150.0f;

	// 検知から攻撃までの時間
	constexpr int kAttackStartTime = 120;

	// 検知を終えるまでの時間
	constexpr int kDetectEndTime = 60;

	// 跳ねるのを終えるまでの時間
	constexpr int kBounceEndTime = 60;

	// 見失う時間
	constexpr int kLostTime = 120;

	// 敵を倒した際のスコア
	constexpr int kKillScore = 200;

	// アニメーション番号
	constexpr int kIdleAnimNum = 5; // 待機
	constexpr int kDetectAnimNum = 1; // 発見
	constexpr int kAttackAnimNum = 0; // 攻撃
	constexpr int kLostAnimNum = 7; // 見失い
	constexpr int kDamageAnimNum = 3; // ダメージ
	constexpr int kBounceAnimNum = 6; // 当たった時のリアクション

	// アニメーションのブレンド時間
	constexpr int kAnimBlendTime = 20;
}

MoveEnemy::MoveEnemy() : 
	EnemyBase(ObjectTag::Enemy, MyLib::ColliderBase::ColliderType::Sphere),
	m_dir{},
	m_detectRadius(0.0f),
	m_detectColor(0),
	m_attackCount(0),
	m_bounceCount(0),
	m_detectEndCount(0),
	m_lostCount(0),
	m_update(&MoveEnemy::IdleUpdate)
{
	// 球の当たり判定を取得
	auto collider = dynamic_pointer_cast<MyLib::SphereCollider>(m_pColliderData);
	// 球の当たり判定の半径を設定
	collider->m_radius = kDefaultRadius;
}

MoveEnemy::~MoveEnemy()
{
}

void MoveEnemy::Init()
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
	m_update = &MoveEnemy::IdleUpdate;

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

void MoveEnemy::End()
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

void MoveEnemy::Update()
{
	// 更新処理を行う
	(this->*m_update)();

	// モデルの更新
	UpdateModel();
}

void MoveEnemy::Draw() const
{
	m_pModel->Draw();
#ifdef _DEBUG
	// デバッグ用の検知範囲を表示
	DrawSphere3D(m_rigidbody.GetPos().ToDxLibVector(), m_detectRadius, 8, m_detectColor, m_detectColor, false);
#endif // _DEBUG
}

void MoveEnemy::OnCollide(std::shared_ptr<Collidable> collider)
{
	// プレイヤーに当たったら
	if (collider->GetTag() == ObjectTag::Player)
	{
		// 現在跳ねておらず、喰らい中でないなら
		if (m_update != &MoveEnemy::BounceUpdate && m_update != &MoveEnemy::DamageUpdate)
		{
			// プレイヤーを取得できたとき
			if (auto player = m_pPlayer.lock())
			{
				// プレイヤーがダメージを受けた場合
				if (player->IsDamage())
				{
					// 当たった時のリアクションを始める
					OnHitReaction();
				}
			}
		}
	}

}

const int MoveEnemy::GetKillScore() const
{
	return kKillScore;
}

void MoveEnemy::SetModel(int handle)
{
	m_pModel->SetModelHandle(MV1DuplicateModel(handle));
	// モデルにアニメーションをセット(Initより前にこれを呼ぶため)
	m_pModel->SetAnimation(kIdleAnimNum, 1.0f, 0, true);
	// モデルのスケールをセット
	m_pModel->SetScale(kModelScale);
}

void MoveEnemy::SetSpawnPos(const Vector3& pos)
{
	m_rigidbody.SetPos(pos);

	// モデルの位置も更新(オフセット分ずらす)
	m_pModel->SetPos(pos - kModelOffset);
}

void MoveEnemy::SetPlayer(std::shared_ptr<Player> player)
{
	m_pPlayer = player;
}

void MoveEnemy::SetBulletManager(std::shared_ptr<BulletManager> manager)
{
	m_pBulletManager = manager;
}

void MoveEnemy::Damage()
{
	// ダメージのSEを再生
	SoundManager::GetInstance().Play("Damage", 1.0f, true);
	// ダメージエフェクトを生成
	EffectManager::GetInstance().CreateEffect(L"hit.efk", m_rigidbody.GetPos());
	// 状態を死亡中のものにする
	m_state = EnemyState::Dying;
	m_update = &MoveEnemy::DamageUpdate;
	m_pModel->SetAnimation(kDamageAnimNum, 0.5f, kAnimBlendTime, false);
}


bool MoveEnemy::IsInDetectRange()
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

void MoveEnemy::Shoot()
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

void MoveEnemy::IdleUpdate()
{

	m_dir = Vector3::LerpVec3(m_dir, kDefaultDir, kLerpSpeed);

	// プレイヤーが検知範囲内に入ったら
	if (IsInDetectRange())
	{
		// 発見した時の処理を行う
		OnDetect();
	}
}

void MoveEnemy::DetectUpdate()
{
	// プレイヤーが取得できるかをチェック
	if (auto player = m_pPlayer.lock())
	{
		// プレイヤーまでの距離を取得
		Vector3 toPlayer = player->GetPos() - m_rigidbody.GetPos();

		// プレイヤーを向くようにする
		m_dir = Vector3::LerpVec3(m_dir, toPlayer.Normalized(), kLerpSpeed);

		// 距離が近すぎる場合止まる
		if (toPlayer.Length() <= kMinDistanceToPlayer)
		{
			m_rigidbody.SetVelocity(Vector3::Zero());
		}
		else
		{
			m_rigidbody.SetVelocity(m_dir * kMoveSpeed);
		}
	}

	m_attackCount++;

	if (m_attackCount >= kAttackStartTime)
	{
		m_attackCount = 0;
		m_update = &MoveEnemy::AttackUpdate;
		m_pModel->SetAnimation(kAttackAnimNum, 0.5f, kAnimBlendTime, false);
	}
	else if (!IsInDetectRange())
	{
		// 見失う時間を設定
		m_lostCount = kLostTime;

		m_attackCount = 0;

		// 敵の速度を止める
		m_rigidbody.SetVelocity(Vector3::Zero());
		// プレイヤー見失い時のエフェクトを生成
		EffectManager::GetInstance().CreateEffect(L"lost.efk", m_rigidbody.GetPos() + kEffectOffset);
		// 更新処理を見失った時のものにする
		m_update = &MoveEnemy::LostUpdate;
		m_pModel->SetAnimation(kLostAnimNum, 0.5f, kAnimBlendTime, true);
	}

}

void MoveEnemy::AttackUpdate()
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
			m_update = &MoveEnemy::DetectUpdate;
			m_pModel->SetAnimation(kDetectAnimNum, 0.5f, kAnimBlendTime, true);
		}
		else
		{
			// 敵の速度を止める
			m_rigidbody.SetVelocity(Vector3::Zero());
			// プレイヤー見失い時のエフェクトを生成
			EffectManager::GetInstance().CreateEffect(L"lost.efk", m_rigidbody.GetPos() + kEffectOffset);
			// 見つからなかったら見失い処理に遷移
			m_lostCount = kLostTime;
			m_update = &MoveEnemy::LostUpdate;
			m_pModel->SetAnimation(kLostAnimNum, 0.5f, kAnimBlendTime, true);
		}
	}
}

void MoveEnemy::LostUpdate()
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
		m_update = &MoveEnemy::IdleUpdate;
		m_pModel->SetAnimation(kIdleAnimNum, 1.0f, kAnimBlendTime, true);
	}

}

void MoveEnemy::DamageUpdate()
{
	// アニメーションが終わったら完全に死亡した状態にする
	if (m_pModel->GetAnimationEnd())
	{
		// 敵が死んだときのエフェクトを生成
		EffectManager::GetInstance().CreateEffect(L"enemyDie.efk", m_rigidbody.GetPos());
		m_state = EnemyState::Dead;
	}
}

void MoveEnemy::BounceUpdate()
{
	m_bounceCount++;

	if (m_bounceCount >= kBounceEndTime)
	{
		// プレイヤーの検知
		if (IsInDetectRange())
		{
			// 見つかったら検知状態に遷移
			m_attackCount = 0;
			m_update = &MoveEnemy::DetectUpdate;
			m_pModel->SetAnimation(kDetectAnimNum, 0.5f, kAnimBlendTime, true);
		}
		else
		{
			// 敵の速度を止める
			m_rigidbody.SetVelocity(Vector3::Zero());
			// プレイヤー見失い時のエフェクトを生成
			EffectManager::GetInstance().CreateEffect(L"lost.efk", m_rigidbody.GetPos() + kEffectOffset);
			// 見つからなかったら見失い処理に遷移
			m_lostCount = kLostTime;
			m_update = &MoveEnemy::LostUpdate;
			m_pModel->SetAnimation(kLostAnimNum, 0.5f, kAnimBlendTime, true);
		}
	}
}

void MoveEnemy::UpdateModel()
{
	// モデルの更新処理を行う
	m_pModel->Update();

	// オフセット分ずらした位置にモデルをセット
	m_pModel->SetPos(m_rigidbody.GetPos() - kModelOffset);

	// モデルの向きをセット
	m_pModel->SetDir(m_dir);
}

void MoveEnemy::OnDetect()
{
	// カメラの範囲外なら処理をしない
	if (!m_isInCamera)return;
	// プレイヤー発見時のエフェクトを生成
	EffectManager::GetInstance().CreateEffect(L"detect.efk", m_rigidbody.GetPos() + kEffectOffset);
	// プレイヤー発見時のSEを再生
	SoundManager::GetInstance().Play("Detect", 1.0f, true);
	// 更新処理を発見時のものにする
	m_update = &MoveEnemy::DetectUpdate;
	m_pModel->SetAnimation(kDetectAnimNum, 0.5f, kAnimBlendTime, true);
}

void MoveEnemy::OnHitReaction()
{
	// 跳ねる状態に遷移
	m_attackCount = 0;
	m_bounceCount = 0;
	// 敵の速度を止める
	m_rigidbody.SetVelocity(Vector3::Zero());
	m_update = &MoveEnemy::BounceUpdate;
	m_pModel->SetAnimation(kBounceAnimNum, 0.5f, kAnimBlendTime, true);
}