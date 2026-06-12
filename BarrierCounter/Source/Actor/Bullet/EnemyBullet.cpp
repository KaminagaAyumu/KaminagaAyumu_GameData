#include "EnemyBullet.h"
#include "../Model.h"
#include "../../Common/Game.h"
#include "../../MyLib/MyLib.h"
#include "../Enemy/EnemyBase.h"
#include "../Enemy/Enemy.h"
#include "../Enemy/MoveEnemy.h"
#include "../Player/Player.h"
#include "../../Sound/SoundManager.h"
#include "../../Effect/EffectManager.h"
#include "../../Effect/Effect.h"

namespace
{
	// 当たり判定の半径
	constexpr float kDefaultRadius = 20.0f;

	// モデルのスケールが大きいので小さくする
	const Vector3 kModelScaleOffset = { 0.25f,0.25f,0.25f };

	// 弾のスピード
	constexpr float kSpeed = 20.0f;

}

EnemyBullet::EnemyBullet() :
	BulletBase(ObjectTag::EnemyBullet, MyLib::ColliderBase::ColliderType::Sphere),
	m_dir{},
	m_update(&EnemyBullet::NormalUpdate)
{
	// 球の当たり判定を取得
	auto collider = dynamic_pointer_cast<MyLib::SphereCollider>(m_pColliderData);
	// 球の当たり判定の半径を設定
	collider->m_radius = kDefaultRadius;
}

EnemyBullet::~EnemyBullet()
{
}

void EnemyBullet::Init()
{
	// Collidableの初期化処理を呼ぶ
	Collidable::Init();

	m_pModel = std::make_shared<Model>();
	m_pModel->Init();

	m_rigidbody.Init();

	// 使うサウンドを登録
	SoundManager::GetInstance().LoadSoundClip("AvoidHit", L"Data/Sound/SE/avoidHit.mp3", SoundBus::SE, 1.0f, false);

	// エフェクトマネージャーのインスタンスを取得
	auto& effectManager = EffectManager::GetInstance();
	// エフェクトを登録
	effectManager.LoadEffect(L"bullet.efk");

	// エフェクトを生成
	m_pEffect = effectManager.CreateEffect(L"bullet.efk", m_rigidbody.GetPos());
}

void EnemyBullet::End()
{
	// Collidableの終了処理を呼ぶ
	Collidable::End();

	m_pModel->End();

	// エフェクトを再生している場合
	if (auto effect = m_pEffect.lock())
	{
		// 現在のエフェクトを止める
		effect->StopEffect();
	}

	// 登録したサウンドを開放
	SoundManager::GetInstance().DeleteSoundClip("AvoidHit");

	// 登録したエフェクトを開放
	EffectManager::GetInstance().DeleteEffect(L"bullet.efk");
}

void EnemyBullet::Update()
{
	(this->*m_update)();
}

void EnemyBullet::Draw() const
{
	m_pModel->Draw();
}

void EnemyBullet::OnCollide(std::shared_ptr<Collidable> collider)
{
	// 弾が回避された状態の時
	if (IsAvoided())
	{
		// 当たった相手が敵かどうかを確認
		if (collider->GetTag() == ObjectTag::Enemy)
		{
			// Collider情報から敵を取得
			// HACK:敵種別ごとに取得できるかどうかを確認
			if (auto enemy = dynamic_pointer_cast<Enemy>(collider))
			{
				// 敵の情報が発射した敵と一致するなら
				if (enemy == m_pOwnerEnemy.lock())
				{
					// 弾を生成した敵を消去
					enemy->Damage();
					// 弾自身も消える
					m_isDead = true;
				}
			}
			else if (auto enemy = dynamic_pointer_cast<MoveEnemy>(collider))
			{
				// 敵の情報が発射した敵と一致するなら
				if (enemy == m_pOwnerEnemy.lock())
				{
					// 弾を生成した敵を消去
					enemy->Damage();
					// 弾自身も消える
					m_isDead = true;
				}
			}
		}
	}
	else
	{
		// 当たった相手がプレイヤーかどうかを確認
		if (collider->GetTag() == ObjectTag::Player)
		{
			// Collider情報からプレイヤーを取得
			if (auto player = dynamic_pointer_cast<Player>(collider))
			{
				// プレイヤーが回避状態なら
				if (player->IsAvoid())
				{
					// 回避された際のSEを再生
					SoundManager::GetInstance().Play("AvoidHit", 1.0f, true);
					// 弾を反射する
					StartReflect();
				}
			}
		}
	}
}

void EnemyBullet::SetModel(int handle)
{
	// モデルをセット
	m_pModel->SetModelHandle(MV1DuplicateModel(handle));
	m_pModel->SetScale(kModelScaleOffset);
}

void EnemyBullet::SetInfo(const Vector3& pos, const Vector3& dir, std::shared_ptr<MyLib::Collidable> owner)
{
	// 向きを取得
	m_dir = dir;
	// 向きのy座標は変わらないようにする
	m_dir.y = 0.0f;

	// 座標と速度を設定
	m_rigidbody.SetPos(pos);
	// 向きに速度を乗算する
	m_rigidbody.SetVelocity(m_dir * kSpeed);
	// モデルに座標をセット
	m_pModel->SetPos(m_rigidbody.GetPos());

	// 敵の情報をCollidableから取得
	auto enemy = dynamic_pointer_cast<EnemyBase>(owner);
	// 生成した敵のポインタを保持する
	m_pOwnerEnemy = enemy;
}

void EnemyBullet::StartReflect()
{
	// 向きを反転
	m_dir *= -1;

	// 向きに速度を乗算する
	m_rigidbody.SetVelocity(m_dir * kSpeed);

	// 回避された状態に変更
	m_update = &EnemyBullet::AvoidedUpdate;
}

void EnemyBullet::NormalUpdate()
{
	// エフェクトを取得できている間は座標の更新をし続ける
	if (auto effect = m_pEffect.lock())
	{
		effect->SetPos(m_rigidbody.GetPos());
	}

	// 生成した敵の情報を取得できない場合消える
	if (!m_pOwnerEnemy.lock())
	{
		m_isDead = true;
	}

	m_pModel->Update();

	m_pModel->SetPos(m_rigidbody.GetPos());

	// マップの外に出た場合は消える
	if (IsOutFromMap()) { m_isDead = true; }
}

void EnemyBullet::AvoidedUpdate()
{
	// 実際の中身はデフォルトの更新処理と同じ
	NormalUpdate();
}

bool EnemyBullet::IsAvoided()
{
	return m_update == &EnemyBullet::AvoidedUpdate;
}
