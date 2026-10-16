#include "BulletEnemy.h"
#include "../../MyLib/Component/Rigidbody.h"
#include "../../MyLib/Component/Animator.h"
#include "../../MyLib/Component/Collision/Collidable.h"
#include "../../MyLib/Component/Draw/Drawable3D.h"
#include "../../MyLib/Component/EffectComponent.h"
#include "../../MyLib/Component/Controller/Enemy/BulletEnemyController.h"
#include <memory>
#include <cassert>

BulletEnemy::BulletEnemy()
{
}

BulletEnemy::~BulletEnemy()
{
}

void BulletEnemy::Init()
{
	AddComponent<MyLib::Animator>();							// Animatorコンポーネントを追加
	AddComponent<MyLib::Rigidbody>();							// Rigidbodyコンポーネントを追加
	AddComponent<MyLib::Collidable>();							// Collidableコンポーネントを追加
	AddComponent<MyLib::Drawable3D>(MyLib::DrawLayer::Opaque);	// 3D描画コンポーネントを追加
	AddComponent<MyLib::BulletEnemyController>();				// BulletEnemyControllerコンポーネントを追加
	AddComponent<MyLib::EffectComponent>();						// エフェクトコンポーネントを追加

	std::shared_ptr<MyLib::BulletEnemyController> pEnemyController = GetComponent<MyLib::BulletEnemyController>().lock();
	pEnemyController->SetPlayer(m_pPlayer);


	// コンポーネントの初期化処理を行う
	GameObject::Init();
}

void BulletEnemy::Update()
{
	// コンポーネントの更新処理を行う
	GameObject::Update();
}

void BulletEnemy::End()
{
	// コンポーネントの終了処理を行う
	GameObject::End();
}

void BulletEnemy::SetPos(const Vector3& pos)
{
	if (std::shared_ptr<MyLib::Transform> pTransform = GetComponent<MyLib::Transform>().lock())
	{
		// 座標を設定する
		pTransform->SetPos(pos);
	}
	else
	{
		assert(false && "BulletEnemy : 初期座標の設定に失敗しました");
	}
}

void BulletEnemy::SetPlayer(std::weak_ptr<MyLib::GameObject> player)
{
	if (std::shared_ptr<MyLib::GameObject> pPlayer = player.lock())
	{
		m_pPlayer = player;
	}
	else
	{
		assert(false && "BulletEnemy : プレイヤーオブジェクトのセットに失敗しました");
	}
}

void BulletEnemy::SetCanAct(bool canAct)
{
	if (auto pController = GetComponent<MyLib::BulletEnemyController>().lock())
	{
		pController->SetCanAct(canAct);
	}
	else
	{
		assert(false && "BulletEnemy : SetCanActに失敗しました");
	}
}
