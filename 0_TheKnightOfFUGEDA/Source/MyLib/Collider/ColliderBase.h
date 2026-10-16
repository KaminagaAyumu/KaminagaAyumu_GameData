#pragma once
#include "../../Geometry/BoundingBox.h"
#include "../../Geometry/Quaternion.h"
#include "../../Geometry/Matrix4x4.h"
#include <string>
#include <functional>
#include <memory>

namespace MyLib
{
	class GameObject;

	/// <summary>
	/// 当たり判定情報を持つ基底クラス
	/// </summary>
	class ColliderBase abstract
	{
	public:

		/// <summary>
		/// オブジェクトのタグ
		/// </summary>
		enum class ObjectTag
		{
			Player,			// プレイヤー
			Enemy,			// 敵
			Stage,			// ステージ
			PlayerAttach,	// プレイヤーについている判定
			EnemyBullet,	// 敵の弾
			Item			// アイテム
		};

		/// <summary>
		/// 当たり判定の形
		/// </summary>
		enum class ColliderShape
		{
			Line,
			Sphere,
			Box,
			Capsule,
			GroundMesh,
			WallMesh
		};

		/// <summary>
		/// 当たった際に受け取る情報
		/// </summary>
		struct HitContext
		{
			bool isJustGuard = false;	// ジャストガードフラグ
			bool isStunned = false;	// 敵がスタン状態かどうか
			std::weak_ptr<GameObject> ownerObj;	// 当たったオブジェクト
		};

		/// <summary>
		/// コンストラクタ
		/// 当たり判定タイプと名前、すり抜けるかどうかを設定
		/// </summary>
		/// <param name="shape">当たり判定の形</param>
		/// <param name="tag">当たり判定のタグ</param>
		/// <param name="name">当たり判定の名前</param>
		/// <param name="isTrigger">すり抜けるかどうか</param>
		ColliderBase(ColliderShape shape, ObjectTag tag, const std::string& name, bool isTrigger);
		virtual ~ColliderBase() = default;

		/// <summary>
		/// 当たり判定タイプを取得する
		/// </summary>
		/// <returns></returns>
		ColliderShape GetShape() const { return m_shape; }

		/// <summary>
		/// 当たり判定の名前を取得する
		/// </summary>
		/// <returns></returns>
		const std::string& GetName() const { return m_name; }

		/// <summary>
		/// オブジェクトのタグを返す
		/// </summary>
		/// <returns></returns>
		ObjectTag GetTag() const { return m_tag; }

		/// <summary>
		/// すり抜けるかどうかを取得する
		/// </summary>
		/// <returns></returns>
		bool IsTrigger() const { return m_isTrigger; }

		bool IsEnable() const { return m_isEnable; }
		void SetEnable(bool isEnable) { m_isEnable = isEnable; }


		/// <summary>
		/// バウンディングボックスを計算する
		/// </summary>
		/// <param name="worldPos">座標</param>
		/// <returns></returns>
		virtual BoundingBox GetBoundingBox(const Vector3& worldPos, const Quaternion& rotation = Quaternion::Identity()) const abstract;

		/// <summary>
		/// デバッグ用の表示を行う
		/// </summary>
		/// <param name="worldPos"></param>
		virtual void DrawDebug(const Vector3& worldPos, const Quaternion& rotation = Quaternion::Identity())const abstract;

		const Vector3& GetLocalOffset() const { return m_localOffset; }
		void SetLocalOffset(const Vector3& offset) { m_localOffset = offset; }
		const Quaternion& GetLocalRotationOffset() const { return m_localRotOffset; }
		void SetLocalRotationOffset(const Quaternion& offset) { m_localRotOffset = offset; }

		/// <summary>
		/// 当たり判定のアンカー(行列)をセットする関数
		/// </summary>
		using AnchorFunc = std::function<Matrix4x4()>;
		void SetAnchor(AnchorFunc func) { m_anchor = std::move(func); }
		void ClearAnchor() { m_anchor = nullptr; }
		bool IsUseAnchor() const { return static_cast<bool>(m_anchor); }
		Matrix4x4 GetAnchorMatrix() const { return m_anchor(); }

		using HitContextFunc = std::function<HitContext()>;
		void SetContext(HitContextFunc func) { m_hitContext = std::move(func); }
		HitContext GetHitContext() const { return m_hitContext ? m_hitContext() : HitContext{}; }

	private:
		// 当たり判定の形
		ColliderShape m_shape;
		// 当たり判定のタグ
		ObjectTag m_tag;
		// 当たり判定の名前
		std::string m_name;
		// すり抜けるかどうか
		bool m_isTrigger;
		// 有効かどうか
		bool m_isEnable;

		// 位置のオフセット関係
		// ローカル位置オフセット
		Vector3 m_localOffset;
		// ローカル回転オフセット
		Quaternion m_localRotOffset;

		// 行列を取得して当たり判定を行う際の関数
		AnchorFunc m_anchor;

		// 当たった際に情報を受け取る際の関数
		HitContextFunc m_hitContext;

	};

}



