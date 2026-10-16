#include "Collidable.h"
#include "../../Collider/SphereCollider.h"
#include "../../ObjectForTree.h"
#include "../../Physics.h"
#include <list>
#include <cassert>

MyLib::Collidable::Collidable() : 
	m_isEnable(true)
{
}

MyLib::Collidable::~Collidable()
{
}

void MyLib::Collidable::Init(std::weak_ptr<MyLib::GameObject> parent)
{
	// 親となるゲームオブジェクトを取得する
	std::shared_ptr<MyLib::GameObject> pParent = parent.lock();

	// 親からTransformコンポーネントの参照を得る
	m_pTransform = pParent->GetComponent <MyLib::Transform>();
	// 親から物理挙動コンポーネントの参照を得る
	m_pRigidbody = pParent->GetComponent<MyLib::Rigidbody>();
	// Rigidbodyが親にない場合assertする
	if (!m_pRigidbody.lock())
	{
		assert(false && "Collidable : Rigidbodyコンポーネントがありません");
	}

	// オブジェクト空間を作成
	m_pOFT = std::make_shared<ObjectForTree>();
	// オブジェクト空間にこの判定をセットする
	m_pOFT->SetOwner(weak_from_this());
	// Physicsクラスに登録する
	Physics::GetInstance().Entry(shared_from_this());
}

void MyLib::Collidable::Start()
{
}

void MyLib::Collidable::Update()
{
}

void MyLib::Collidable::End()
{
	// セルから当たり判定を削除する
	m_pOFT->RemoveFromCell();
	// Physicsクラスから削除する
	Physics::GetInstance().Exit(shared_from_this());
}

void MyLib::Collidable::AddCollider(std::shared_ptr<MyLib::ColliderBase> collider)
{
	// コライダーが生成されていたら追加する
	if (collider)
	{
		m_pColliders.push_back(collider);
	}
}

MyLib::WorldInfo MyLib::Collidable::GetWorldInfo(const std::shared_ptr<ColliderBase>& collider)
{
	// コライダーが行列アンカーを使用する場合
	if (collider->IsUseAnchor())
	{
		WorldInfo info;
		// 行列アンカーを取得する
		Matrix4x4 anchor = collider->GetAnchorMatrix();

		// 行列アンカーの座標と回転を取得
		info.pos = anchor.GetPosition();
		info.rotation = anchor.GetRotation();
		return info;
	}

	// TransformとRigidbodyを取得する
	std::shared_ptr<Transform> pTransform = m_pTransform.lock();
	std::shared_ptr<Rigidbody> pRigidbody = m_pRigidbody.lock();

	// 取得できなかった場合アサート
	if (!pTransform || !pRigidbody)
	{
		assert(false && "Collidable : ワールド情報を取得できませんでした");
	}

	// 変換前の座標
	Vector3 basePos = pRigidbody->GetNextPos();
	// 変換前の回転
	Quaternion worldRot = pTransform->GetRotation();
	// 返す情報
	WorldInfo info;
	// 座標を回転した上で元の座標に加算
	info.pos = basePos + worldRot * collider->GetLocalOffset();
	// 回転を変換
	info.rotation = worldRot * collider->GetLocalRotationOffset();

	return info;
}

BoundingBox MyLib::Collidable::GetBoundingBox()
{
	std::shared_ptr<MyLib::Rigidbody> pRigidbody = m_pRigidbody.lock();

	if(!pRigidbody)
	{
		assert(false && "Collidable : Rigidbodyのデータが入っていません");
	}

	bool isFirst = true;
	BoundingBox ret;
	for (auto& collider : m_pColliders)
	{
		// 当たり判定が非表示ならば処理をしない
		if (!collider->IsEnable()) continue;

		// ワールド情報を取得
		WorldInfo info = GetWorldInfo(collider);

		// 最初のバウンディングボックスならば新しく生成する
		if (isFirst)
		{
			ret = collider->GetBoundingBox(info.pos, info.rotation);
			isFirst = false;
		}
		else
		{
			ret.Extend(collider->GetBoundingBox(info.pos, info.rotation));
		}
	}

	return ret;
}

void MyLib::Collidable::OnCollide(const CollisionInfo& info)
{
	// 当たった時の関数が登録されていれば実行する
	if (m_onCollide)
	{
		m_onCollide(info);
	}
}

void MyLib::Collidable::OnCollideEnter(const CollisionInfo& info)
{
	// 当たった瞬間の関数が登録されていれば実行する
	if (m_onCollideEnter)
	{
		m_onCollideEnter(info);
	}
}

void MyLib::Collidable::OnCollideExit(const CollisionInfo& info)
{
	// 当たらなくなった瞬間の関数が登録されていれば実行する
	if (m_onCollideExit)
	{
		m_onCollideExit(info);
	}
}
