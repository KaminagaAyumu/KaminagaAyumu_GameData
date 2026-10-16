#pragma once
#include "../Geometry/Vector3.h"
#include "Component/Component.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <typeindex>

namespace MyLib
{
	/// <summary>
	/// ゲーム中に存在するオブジェクト
	/// </summary>
	class GameObject : public std::enable_shared_from_this<GameObject>
	{
	public:
		/// <summary>
		/// オブジェクトの種類
		/// </summary>
		enum class Type
		{
			Player,		// プレイヤー
			Enemy,		// 敵
			Item,		// アイテム
			Projectile,	// 飛び道具
			UI,			// UI
			Camera,		// カメラ
			BackGround,	// 背景
		};

	public:

		/// <summary>
		/// コンストラクタ
		/// デフォルトでTransformコンポーネントがAddされています
		/// </summary>
		explicit GameObject(Type type);
		virtual ~GameObject();

		/// <summary>
		/// オブジェクトにコンポーネントを追加する
		/// </summary>
		/// <typeparam name="ComponentType">コンポーネントの種類</typeparam>
		/// <returns>コンポーネントの弱参照</returns>
		template<typename ComponentType, typename... Args>
		std::weak_ptr<ComponentType> AddComponent(Args&&... args)
		{
			// マップに対応するキーを取得
			std::type_index key = std::type_index(typeid(ComponentType));

			// すでに同じ型が登録されている場合は上書きしない
			if (m_components.count(key)) return GetComponent<ComponentType>();

			// コンポーネントを作成
			std::shared_ptr<ComponentType> newComp = std::make_shared<ComponentType>(std::forward<Args>(args)...);

			// コンポーネントリストに登録
			m_components[key] = newComp;

			// コンポーネントを返す
			return std::static_pointer_cast<ComponentType>(m_components[key]);
		}

		/// <summary>
		/// オブジェクトのコンポーネントを取得する
		/// </summary>
		/// <typeparam name="ComponentType">コンポーネントの種類</typeparam>
		/// <returns>コンポーネントの弱参照</returns>
		template<typename ComponentType>
		std::weak_ptr<ComponentType> GetComponent()
		{
			// コンポーネントが存在するかを確認する
			auto it = m_components.find(std::type_index(typeid(ComponentType)));

			// コンポーネントが存在する場合
			if (it != m_components.end())
			{
				// キーに対応するコンポーネントを返す
				return std::static_pointer_cast<ComponentType>(it->second);
			}
			// コンポーネントが存在しない場合空のポインタを返す
			return std::weak_ptr<ComponentType>();
		}

		virtual void Init();
		
		virtual void Update();

		virtual void End();

		/// <summary>
		/// オブジェクトの種類を返す
		/// </summary>
		/// <returns>オブジェクトの種類</returns>
		Type GetType() { return m_type; }

		/// <summary>
		/// オブジェクトを消去する
		/// </summary>
		void Destroy() { m_isDestroyed = true; }

		/// <summary>
		/// 削除されたかどうかを判別する
		/// </summary>
		/// <returns>true : 削除された false : 削除されていない</returns>
		bool IsDestroyed()const { return m_isDestroyed; }

	protected:
		// 必要なコンポーネントのリスト
		std::unordered_map<std::type_index, std::shared_ptr<Component>> m_components;

	private:
		// 削除フラグ
		bool m_isDestroyed;

		// ゲームオブジェクトのタイプ
		Type m_type;
	};
}