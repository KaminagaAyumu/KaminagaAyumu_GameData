#pragma once
#include <cstdint>
#include "../../../Geometry/Vector3.h"
#include "../Component.h"
#include <memory>

namespace MyLib
{
	/// <summary>
	/// 描画するレイヤー
	/// </summary>
	enum class DrawLayer : uint8_t
	{
		BackGround,		// 背景
		Opaque,			// 不透明
		Transparent,	// 透明
		Effect,			// エフェクト
		UI,				// UI
		Max				// レイヤーの最大数
	};

	/// <summary>
	/// 描画インターフェースコンポーネント
	/// 派生クラスでComponentの関数であるInit,Update,Endを必ず継承させる
	/// </summary>
	class Drawable abstract : public MyLib::Component, public std::enable_shared_from_this<Drawable>
	{
	public:
		/// <summary>
		/// コンストラクタ
		/// 必ずレイヤー情報をセットする
		/// </summary>
		explicit Drawable(DrawLayer layer);

		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~Drawable() = default;

		/// <summary>
		/// 描画処理
		/// </summary>
		virtual void Draw() const abstract;

		/// <summary>
		/// 描画するレイヤー情報を取得する
		/// 継承したクラスで戻り値としてレイヤー番号を返す
		/// </summary>
		/// <returns>描画するレイヤー情報</returns>
		virtual DrawLayer GetDrawLayer() { return m_layer; }

		/// <summary>
		/// 深度を取得する
		/// 3Dの半透明物体の描画の際に使用する
		/// </summary>
		/// <returns>深度(デフォルトは0.0f)</returns>
		virtual float GetDepth() { return 0.0f; }

		/// <summary>
		/// シャドウマップを行うかどうか(影を落とす側)
		/// </summary>
		/// <returns></returns>
		virtual bool IsCastShadow() { return false; }

		/// <summary>
		/// シャドウマップを行うかどうか(影を落とされる側)
		/// </summary>
		/// <returns></returns>
		virtual bool IsReceiveShadow() { return false; }

	private:
		// 使用するレイヤー(最初に設定したものからは変えられない)
		DrawLayer m_layer;

	};
}

