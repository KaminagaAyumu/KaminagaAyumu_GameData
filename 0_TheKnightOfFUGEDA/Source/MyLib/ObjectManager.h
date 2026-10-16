#pragma once
#include <memory>
#include <list>

namespace MyLib
{
	// プロトタイプ宣言
	class GameObject;

	/// <summary>
	/// ゲームオブジェクトを管理するクラス
	/// </summary>
	class ObjectManager
	{
	public:
		virtual ~ObjectManager();

		/// <summary>
		/// インスタンスを取得する
		/// </summary>
		/// <returns></returns>
		static ObjectManager& GetInstance();

		/// <summary>
		/// オブジェクトを追加する
		/// </summary>
		/// <param name="object"></param>
		void AddObject(std::shared_ptr<GameObject> object);

		/// <summary>
		/// 初期化処理
		/// GameObjectの初期化を行う
		/// </summary>
		void Init();

		/// <summary>
		/// 更新処理
		/// </summary>
		void Update();

		/// <summary>
		/// 終了処理
		/// 残っているGameObjectの終了処理を行う
		/// </summary>
		void End();

		/// <summary>
		/// 更新状態を変更する
		/// </summary>
		/// <param name="isUpdate">true : 更新を行う false : 更新を行わない</param>
		void SetUpdate(bool isUpdate) { m_isUpdate = isUpdate; }

	private:
		std::list<std::shared_ptr<GameObject>> m_pGameObjects; // 管理するGameObjectクラス

		bool m_isUpdate;	// 更新を止めるかどうかのフラグ

	private:
		/// <summary>
		/// コンストラクタ
		/// シングルトンクラスのためprivateで宣言する
		/// ※宣言の際にcppファイルの一番上に配置しています
		/// </summary>
		ObjectManager();
		ObjectManager(const ObjectManager&) = delete; // コピーコンストラクタを作れないようにする
		void operator=(const ObjectManager&) = delete; // 代入演算子も使えないようにする
	};

}



