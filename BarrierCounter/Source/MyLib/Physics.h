#pragma once
#include "Collidable.h"
#include <list>
#include <memory>

namespace MyLib
{

	/// <summary>
	/// 物理挙動クラス
	/// 継承不可
	/// </summary>
	class Physics final
	{
	public:
		// デストラクタ
		// 処理なし
		virtual ~Physics() = default;

		/// <summary>
		/// インスタンスを取得する
		/// </summary>
		/// <returns></returns>
		static Physics& GetInstance();

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

		/// <summary>
		/// タイムスケールをセットする
		/// </summary>
		/// <param name="timeScale">タイムスケール</param>
		void SetTimeScale(float timeScale) { m_timeScale = timeScale; }

	private:
		std::list<std::shared_ptr<MyLib::Collidable>> m_pCollidables; // 登録されたCollidableクラス

		// 更新時間のスケール
		float m_timeScale;

		// 更新を止めるかどうか
		bool m_isStop;

	private:
		Physics(); // コンストラクタをprivateで宣言
		Physics(const Physics&) = delete; // コピーコンストラクタを作れないようにする
		void operator=(const Physics&) = delete; // 代入演算子も使えないようにする

	};

}



