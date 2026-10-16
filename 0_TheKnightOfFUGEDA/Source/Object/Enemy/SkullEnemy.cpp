#include "SkullEnemy.h"
#include "../../MyLib/Component/Rigidbody.h"
#include "../../MyLib/Component/Animator.h"
#include "../../MyLib/Component/Collision/Collidable.h"
#include "../../MyLib/Component/Draw/Drawable3D.h"
#include "../../MyLib/Component/EffectComponent.h"
#include "../../MyLib/Component/Controller/Enemy/SkullEnemyController.h"
#include <memory>
#include <cassert>

namespace
{
	// モデルのサイズ
	const Vector3 kModelScale = { 0.01f, 0.01f, 0.01f };
}

SkullEnemy::SkullEnemy()
{
}

SkullEnemy::~SkullEnemy()
{
}

void SkullEnemy::Init()
{
	AddComponent<MyLib::Animator>();							// Animatorコンポーネントを追加
	AddComponent<MyLib::Rigidbody>();							// Rigidbodyコンポーネントを追加
	AddComponent<MyLib::Collidable>();							// Collidableコンポーネントを追加
	AddComponent<MyLib::Drawable3D>(MyLib::DrawLayer::Opaque);	// 3D描画コンポーネントを追加
	AddComponent<MyLib::SkullEnemyController>();				// SkullEnemyControllerコンポーネントを追加
	AddComponent<MyLib::EffectComponent>();						// エフェクトコンポーネントを追加

	std::shared_ptr<MyLib::SkullEnemyController> pEnemyController = GetComponent<MyLib::SkullEnemyController>().lock();
	pEnemyController->SetPlayer(m_pPlayer);


	// コンポーネントの初期化処理を行う
	GameObject::Init();
}

void SkullEnemy::Update()
{
	// コンポーネントの更新処理を行う
	GameObject::Update();
}

void SkullEnemy::End()
{
	// コンポーネントの終了処理を行う
	GameObject::End();
}

void SkullEnemy::SetPos(const Vector3& pos)
{
	if (std::shared_ptr<MyLib::Transform> pTransform = GetComponent<MyLib::Transform>().lock())
	{
		// 座標を設定する
		pTransform->SetPos(pos);
	}
	else
	{
		assert(false && "SkullEnemy : 初期座標の設定に失敗しました");
	}
}

void SkullEnemy::SetPlayer(std::weak_ptr<MyLib::GameObject> player)
{
	if (std::shared_ptr<MyLib::GameObject> pPlayer = player.lock())
	{
		m_pPlayer = player;
	}
	else
	{
		assert(false && "SkullEnemy : プレイヤーオブジェクトのセットに失敗しました");
	}
}

void SkullEnemy::SetCanAct(bool canAct)
{
	if (auto pController = GetComponent<MyLib::SkullEnemyController>().lock())
	{
		pController->SetCanAct(canAct);
	}
	else
	{
		assert(false && "SkullEnemy : SetCanActに失敗しました");
	}
}