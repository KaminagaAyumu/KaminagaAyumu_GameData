#pragma once
#include "../Component.h"
#include "../Transform.h"
#include "../Rigidbody.h"
#include "../../MyStruct.h"
#include "../../GameObject.h"
#include "../../../Geometry/BoundingBox.h"
#include "../../Collider/ColliderBase.h"
#include <memory>
#include <vector>
#include <functional>

namespace MyLib
{
	class ObjectForTree;
	class Collidable;

	/// <summary>
	/// 当たった際の情報
	/// </summary>
	struct CollisionInfo
	{
		std::shared_ptr<Collidable> otherCollidable;	// 相手のCollidable
		std::shared_ptr<ColliderBase> myCollider;		// 自分のCollider
		std::shared_ptr<ColliderBase> otherCollider;	// 相手のCollider
		Vector3 hitNormal = Vector3::Zero();
	};
	
	/// <summary>
	/// 当たり判定クラス
	/// </summary>
	class Collidable : public Component, public std::enable_shared_from_this<Collidable>
	{
	public:
		// 当たり判定関数
		using OnCollideCallBack = std::function<void(const CollisionInfo&)>;
	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		Collidable();
		virtual ~Collidable();

		virtual void Init(std::weak_ptr<MyLib::GameObject> parent) override;
		virtual void Start() override;
		virtual void Update() override;
		virtual void End() override;

		/// <summary>
		/// 当たり判定を追加する
		/// コライダーをこの関数の引数でmake_sharedしてください
		/// </summary>
		/// <param name="collider">指定のコライダー</param>
		void AddCollider(std::shared_ptr<ColliderBase> collider);

		/// <summary>
		/// 当たった際の処理をセットする
		/// コンポーネント側でセットしてください
		/// </summary>
		/// <param name="callBack">当たった時の関数</param>
		void SetOnCollide(OnCollideCallBack callBack) { m_onCollide = callBack; }

		/// <summary>
		/// 当たった瞬間の処理をセットする
		/// コンポーネント側でセットしてください
		/// </summary>
		/// <param name="callBack">当たった瞬間の関数</param>
		void SetOnCollideEnter(OnCollideCallBack callBack) { m_onCollideEnter = callBack; }
		
		/// <summary>
		/// 当たらなくなった瞬間の処理をセットする
		/// コンポーネント側でセットしてください
		/// </summary>
		/// <param name="callBack">当たらなくなった瞬間の関数</param>
		void SetOnCollideExit(OnCollideCallBack callBack) { m_onCollideExit = callBack; }

		/// <summary>
		/// 当たり判定の有効状態を設定する
		/// </summary>
		/// <param name="isEnable">true : 当たり判定を行う false : 当たり判定を行わない</param>
		void SetEnable(bool isEnable) { m_isEnable = isEnable; }

		WorldInfo GetWorldInfo(const std::shared_ptr<ColliderBase>& collider);

		/// <summary>
		/// AABB矩形を返す
		/// </summary>
		/// <returns></returns>
		BoundingBox GetBoundingBox();

		/// <summary>
		/// オブジェクトが属している空間を取得する
		/// </summary>
		/// <returns></returns>
		std::weak_ptr<ObjectForTree> GetOFT() { return m_pOFT; }

		/// <summary>
		/// 当たった時の処理
		/// </summary>
		/// <param name="info">Colliderの情報</param>
		void OnCollide(const CollisionInfo& info);
		
		/// <summary>
		/// 当たった瞬間の処理
		/// </summary>
		/// <param name="info">Colliderの情報</param>
		void OnCollideEnter(const CollisionInfo& info);

		/// <summary>
		/// 当たらなくなった瞬間の処理
		/// </summary>
		/// <param name="info">Colliderの情報</param>
		void OnCollideExit(const CollisionInfo& info);

		bool IsEnable()const { return m_isEnable; }

		const std::vector<std::shared_ptr<ColliderBase>>& GetColliders() const { return m_pColliders; }

		std::weak_ptr<Transform> GetTransform() const { return m_pTransform; }

		std::weak_ptr<Rigidbody> GetRigidbody() const { return m_pRigidbody; }

	protected:

		// 位置関連データ
		std::weak_ptr<MyLib::Transform> m_pTransform;

		// 物理挙動関連データ
		std::weak_ptr<MyLib::Rigidbody> m_pRigidbody;

		// 当たり判定関連データ
		std::vector<std::shared_ptr<ColliderBase>> m_pColliders;

		// 空間管理データ
		std::shared_ptr<ObjectForTree> m_pOFT;

		// 当たった際の処理を呼ぶための関数オブジェクト
		OnCollideCallBack m_onCollide;

		// 当たった瞬間の処理を呼ぶための関数オブジェクト
		OnCollideCallBack m_onCollideEnter;
		
		// 当たらなくなった瞬間の処理を呼ぶための関数オブジェクト
		OnCollideCallBack m_onCollideExit;

		// 当たり判定が有効かどうかのフラグ
		bool m_isEnable;
	};
}


