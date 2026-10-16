#pragma once
#include "Rigidbody.h"
#include "ColliderBase.h"
#include "../Common/ObjectTag.h"
#include <memory>

namespace MyLib
{
	class Physics;
	class ColliderBase;

	/// <summary>
	/// 衝突可能な物体
	/// 自身をshared_ptr化できるようにする
	/// </summary>
	class Collidable abstract : public std::enable_shared_from_this<Collidable>
	{
	public:
		Collidable(ObjectTag tag, ColliderBase::ColliderType type);
		virtual ~Collidable();

		/// <summary>
		/// 初期化処理
		/// physicsに登録する
		/// </summary>
		void Init();

		/// <summary>
		/// 終了処理
		/// physicsから解除する
		/// </summary>
		void End();

		/// <summary>
		/// 当たった時の処理
		/// </summary>
		/// <param name="collider">当たったオブジェクト</param>
		virtual void OnCollide(std::shared_ptr<Collidable> collider) abstract;

		/// <summary>
		/// マップの外に出たかどうか
		/// </summary>
		/// <returns></returns>
		bool IsOutFromMap()const;

		/// <summary>
		/// オブジェクトのタグを返す
		/// </summary>
		/// <returns></returns>
		ObjectTag GetTag() const { return m_tag; };

	private:
		// Physicsで物理処理を行うためフレンド化
		friend Physics;

		/// <summary>
		/// 当たり判定を作成する
		/// </summary>
		/// <param name="type">当たり判定の種類</param>
		void CreateCollider(ColliderBase::ColliderType type);

	private:

		// オブジェクトのタグを設定する
		ObjectTag m_tag;

	protected:
		// 物理挙動関連データ
		Rigidbody m_rigidbody;
		// 当たり判定関連データ
		std::shared_ptr<ColliderBase> m_pColliderData;
		// 未来の座標
		Vector3 m_nextPos;

	};
}

