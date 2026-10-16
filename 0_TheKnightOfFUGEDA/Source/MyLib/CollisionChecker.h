#pragma once
#include "MyStruct.h"
#include <vector>
#include <memory>
#include <map>

namespace MyLib
{
	class Collidable;
	class ColliderBase;

	/// <summary>
	/// 当たり判定管理系クラス
	/// </summary>
	class CollisionChecker
	{
	public:

		/// <summary>
		/// 当たり判定の情報
		/// </summary>
		struct ContactsInfo
		{
			Vector3 normal = Vector3::Zero();
			float depth = 0.0f;
			Vector3 point; // 当たった地点
		};

		/// <summary>
		/// 当たり判定結果
		/// </summary>
		struct ColResult
		{
			bool isHit = false;
			std::vector<ContactsInfo> contacts;
		};

		using CollisionPairKey = std::pair<ColliderBase*, ColliderBase*>;

		struct CollisionData
		{
			std::weak_ptr<Collidable> collidableA;
			std::weak_ptr<Collidable> collidableB;
			std::weak_ptr<ColliderBase> colliderA;
			std::weak_ptr<ColliderBase> colliderB;
		};

	public:

		CollisionChecker();

		/// <summary>
		/// 当たり判定のペアの判定を行う
		/// </summary>
		/// <param name="pairs"></param>
		void CheckCollisions(const std::vector<std::pair<std::shared_ptr<Collidable>, std::shared_ptr<Collidable>>>& pairs);

		static ColResult IsColliding(std::shared_ptr<ColliderBase> colA, std::shared_ptr<Collidable> collidableA, std::shared_ptr<ColliderBase> colB, std::shared_ptr<Collidable> collidableB);
		static ColResult IsColSphereSphere(std::shared_ptr<ColliderBase> colA, std::shared_ptr<Collidable> collidableA, std::shared_ptr<ColliderBase> colB, std::shared_ptr<Collidable> collidableB);
		static ColResult IsColCapsuleCapsule(std::shared_ptr<ColliderBase> colA, std::shared_ptr<Collidable> collidableA, std::shared_ptr<ColliderBase> colB, std::shared_ptr<Collidable> collidableB);
		static ColResult IsColSphereCapsule(std::shared_ptr<ColliderBase> colA, std::shared_ptr<Collidable> collidableA, std::shared_ptr<ColliderBase> colB, std::shared_ptr<Collidable> collidableB);
		static ColResult IsColSphereGroundMesh(std::shared_ptr<ColliderBase> colA, std::shared_ptr<Collidable> collidableA, std::shared_ptr<ColliderBase> colB, std::shared_ptr<Collidable> collidableB);
		static ColResult IsColSphereWallMesh(std::shared_ptr<ColliderBase> colA, std::shared_ptr<Collidable> collidableA, std::shared_ptr<ColliderBase> colB, std::shared_ptr<Collidable> collidableB);

		/// <summary>
		/// 当たり判定が消える時にExit処理を呼ぶ
		/// </summary>
		/// <param name="collidable">対象の当たり判定コンポーネント</param>
		void ApplyDestroyed(std::shared_ptr<Collidable> collidable);

	private:

		CollisionPairKey CreatePairKey(ColliderBase* colliderA, ColliderBase* colliderB);

	private:
		// 現在当たっているコライダー
		std::map<CollisionPairKey, CollisionData> m_currentColData;
	};

}



