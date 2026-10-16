#include "Item.h"
#include "../../MyLib/Component/Rigidbody.h"
#include "../../MyLib/Component/Animator.h"
#include "../../MyLib/Component/Collision/Collidable.h"
#include "../../MyLib/Component/Draw/Drawable3D.h"
#include "../../MyLib/Component/EffectComponent.h"
#include "../../MyLib/Component/Controller/ItemController.h"
#include <cassert>

Item::Item() : 
	GameObject(MyLib::GameObject::Type::Item)
{
}

Item::~Item()
{
}

void Item::Init()
{
	AddComponent<MyLib::Rigidbody>();							// Rigidbodyコンポーネントを追加
	AddComponent<MyLib::Collidable>();							// Collidableコンポーネントを追加
	AddComponent<MyLib::Drawable3D>(MyLib::DrawLayer::Opaque);	// 3D描画コンポーネントを追加
	AddComponent<MyLib::ItemController>();						// ItemControllerコンポーネントを追加
	AddComponent<MyLib::EffectComponent>();						// エフェクトコンポーネントを追加

	// コンポーネントの初期化処理を行う
	GameObject::Init();
}

void Item::Update()
{
	// コンポーネントの更新処理を行う
	GameObject::Update();
}

void Item::End()
{
	// コンポーネントの終了処理を行う
	GameObject::End();
}

void Item::SetPos(const Vector3& pos)
{
	if (std::shared_ptr<MyLib::Transform> pTransform = GetComponent<MyLib::Transform>().lock())
	{
		// 座標を設定する
		pTransform->SetPos(pos);
	}
	else
	{
		assert(false && "Item : 初期座標の設定に失敗しました");
	}
}

bool Item::IsDestroy()
{
	return GetComponent<MyLib::ItemController>().lock()->IsDestroy();
}