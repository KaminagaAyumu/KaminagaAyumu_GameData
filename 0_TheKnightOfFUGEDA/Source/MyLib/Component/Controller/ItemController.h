#pragma once
#include "../Component.h"
#include <memory>

namespace MyLib
{
	class Transform;
	class Drawable3D;
	class GameObject;
	class ItemController : public Component
	{
	public:

		ItemController();
		virtual ~ItemController();

		void Init(std::weak_ptr<MyLib::GameObject> parent) override;
		void Start() override;
		void Update() override;
		void End() override;

		/// <summary>
		/// 消去するかどうかを返す
		/// </summary>
		/// <returns></returns>
		bool IsDestroy() { return m_isDestroy; }

	private:
		// 回転角度
		float m_angle;
		// 浮遊しているように見せる際に使う値
		float m_wave;
		// 消去する際のフラグ
		bool m_isDestroy;
		// 消去するまでの時間カウンタ
		int m_destroyFrame;
		// 回転、浮遊の際に使用するモデルのオフセット用Transform
		std::shared_ptr<MyLib::Transform> m_pModelOffset;
		// モデルを動かすために取得するオブジェクトの3D描画コンポーネント
		std::weak_ptr<MyLib::Drawable3D> m_pDrawable;
		// オブジェクトのTransform
		std::weak_ptr<MyLib::Transform> m_pTransform;
		// 自身のGameObjectの参照
		std::weak_ptr<MyLib::GameObject> m_pGameObj;
	};

}

