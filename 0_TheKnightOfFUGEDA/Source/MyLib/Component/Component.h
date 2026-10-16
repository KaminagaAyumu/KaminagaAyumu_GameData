#pragma once
#include <memory>
#include <list>

namespace MyLib
{
	// プロトタイプ宣言
	class GameObject;

	/// <summary>
	/// コンポーネントの基底クラス
	/// </summary>
	class Component abstract
	{
	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		Component();

		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~Component() = default;

		/// <summary>
		/// 初期化処理
		/// 必要なコンポーネントの情報を得るために
		/// 親となるオブジェクトを参照させる
		/// </summary>
		/// <param name=""></param>
		virtual void Init(std::weak_ptr<GameObject>) abstract;

		/// <summary>
		/// 初期化処理2
		/// 他のコンポーネントに干渉しなければならない場合は、ここで行う
		/// </summary>
		virtual void Start() abstract;

		/// <summary>
		/// 更新処理
		/// </summary>
		virtual void Update() abstract;

		/// <summary>
		/// 終了処理
		/// </summary>
		virtual void End() abstract;


	private:
		Component(const Component&) = delete;		// コピーコンストラクタを作れないようにする
		void operator=(const Component&) = delete;	// 代入演算子を使えないようにする

		// HACK: 情報が欲しい他のコンポーネントのリストがあると
		// 共通依存関係などができた時には便利かも？
	};
}



