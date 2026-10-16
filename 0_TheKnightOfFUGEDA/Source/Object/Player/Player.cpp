#include "Player.h"
#include "../../MyLib/Component/Transform.h"
#include "../../MyLib/Component/Rigidbody.h"
#include "../../MyLib/Component/Animator.h"
#include "../../MyLib/Component/Collision/Collidable.h"
#include "../../MyLib/Component/Draw/Drawable3D.h"
#include "../../MyLib/Component/EffectComponent.h"
#include "../../MyLib/Component/Controller/Player/PlayerController.h"
#include "../../MyLib/Collider/CapsuleCollider.h"
#include "../../Geometry/Quaternion.h"
#include <cassert>
#include <memory>

Player::Player() : 
	GameObject(MyLib::GameObject::Type::Player)
{
}

Player::~Player()
{
}

void Player::Init()
{
	AddComponent<MyLib::Animator>();							// Animatorコンポーネントを追加
	AddComponent<MyLib::Rigidbody>();							// Rigidbodyコンポーネントを追加
	AddComponent<MyLib::Collidable>();							// Collidableコンポーネントを追加
	AddComponent<MyLib::Drawable3D>(MyLib::DrawLayer::Opaque);	// 3D描画コンポーネントを追加
	AddComponent<MyLib::PlayerController>();					// PlayerControllerコンポーネントを追加
	AddComponent<MyLib::EffectComponent>();						// エフェクトコンポーネントを追加

	// コンポーネントの初期化処理を行う
	GameObject::Init();
}

void Player::Update()
{
	// コンポーネントの更新処理を行う
	GameObject::Update();
}

void Player::End()
{
	// コンポーネントの終了処理を行う
	GameObject::End();
}

const Vector3& Player::GetPos()
{
	auto pTransform = GetComponent<MyLib::Transform>().lock();
	return pTransform->GetPos();
}

int Player::GetLife()
{
	if (auto controller = GetComponent<MyLib::PlayerController>().lock())
	{
		return controller->GetLife();
	}
	else
	{
		assert(false && "Player : GetLifeに失敗しました");
	}
	return 0;
}

int Player::GetMaxLife()
{
	if (auto controller = GetComponent<MyLib::PlayerController>().lock())
	{
		return controller->GetMaxLife();
	}
	else
	{
		assert(false && "Player : GetMaxLifeに失敗しました");
	}
	return 0;
}

void Player::SetCanAct(bool canAct)
{
	if (auto controller = GetComponent<MyLib::PlayerController>().lock())
	{
		controller->SetCanAct(canAct);
	}
	else
	{
		assert(false && "Player : SetCanActに失敗しました");
	}
}

void Player::SetLockOnTarget(std::weak_ptr<MyLib::Transform> target)
{
	if (auto controller = GetComponent<MyLib::PlayerController>().lock())
	{
		controller->SetLockOnTarget(target);
	}
	else
	{
		assert(false && "Player : SetLockOnTargetに失敗しました");
	}
}

void Player::ClearLockOn()
{
	if (auto controller = GetComponent<MyLib::PlayerController>().lock())
	{
		controller->ClearLockOnTarget();
	}
	else
	{
		assert(false && "Player : ClearLockOnTargetに失敗しました");
	}
}

bool Player::IsDied()
{
	if (auto controller = GetComponent<MyLib::PlayerController>().lock())
	{
		return controller->IsPlayerDie();
	}
	else
	{
		assert(false && "Player : IsDiedのチェックにに失敗しました");
		return false;
	}
}
