#pragma once
#include "Component/Collision/Collidable.h"
#include "OctreeManager.h"
#include "MyStruct.h"
#include <memory>
#include <list>


namespace MyLib
{
	class ColliderBase;
	class Transform;
	class CollisionChecker;
	/// <summary>
	/// 物理挙動クラス
	/// 継承不可
	/// </summary>
	class Physics final
	{
	public:
		// デストラクタ
		// 処理なし
		~Physics() = default;

		/// <summary>
		/// インスタンスを取得する
		/// </summary>
		/// <returns></returns>
		static Physics& GetInstance();

		/// <summary>
		/// 初期化をする
		/// </summary>
		/// <param name="level"></param>
		/// <param name="worldAABB"></param>
		void Init(uint32_t level, const BoundingBox& worldAABB);

		/// <summary>
		/// 衝突物の登録
		/// </summary>
		/// <param name="collidable">登録する衝突物</param>
		void Entry(std::shared_ptr<MyLib::Collidable> collidable);

		/// <summary>
		/// 衝突物の解除
		/// </summary>
		/// <param name="collidable">解除する衝突物</param>
		void Exit(std::shared_ptr<MyLib::Collidable> collidable);

		/// <summary>
		/// 押し戻し情報を登録する
		/// </summary>
		/// <param name="info"></param>
		void RegisterPushBackInfo(PushBackInfo info);

		/// <summary>
		/// 物理更新
		/// </summary>
		void Update();

		/// <summary>
		/// 物理更新を止める
		/// </summary>
		void StopUpdate() { m_isStop = true; }

		/// <summary>
		/// 物理更新を行う
		/// </summary>
		void StartUpdate() { m_isStop = false; }

	private:

		void UpdateRigidbody();

		/// <summary>
		/// 再度八分木空間に当たり判定を登録する
		/// </summary>
		void ReregisterAll();

		/// <summary>
		/// 押し戻しを設定する
		/// </summary>
		void ApplyPushBacks();

		/// <summary>
		/// 座標情報を更新する
		/// </summary>
		void UpdatePositions();

	private:
		std::list<std::shared_ptr<Collidable>> m_pCollidables; // 登録されたCollidableクラス

		std::vector<PushBackInfo> m_pPushBackInfos;	// 押し戻しする情報

		std::unique_ptr<CollisionChecker> m_pCollisionChecker;	// 当たり判定用のクラス

		OctreeManager m_octreeManager;

		// 更新を止めるかどうか
		bool m_isStop;

	private:
		/// <summary>
		/// コンストラクタ
		/// シングルトンクラスのためprivateで宣言する
		/// ※宣言の際にcppファイルの一番上に配置しています
		/// </summary>
		Physics();
		Physics(const Physics&) = delete; // コピーコンストラクタを作れないようにする
		void operator=(const Physics&) = delete; // 代入演算子も使えないようにする
	};
}

