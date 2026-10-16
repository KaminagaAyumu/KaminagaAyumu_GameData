#pragma once
#include "../Component.h"
#include "../../../Geometry/Vector3.h"
#include <memory>

namespace MyLib
{
	class Transform;

	/// <summary>
	/// カメラに必要な要素をまとめたコンポーネント
	/// </summary>
	class CameraComponent final : public Component, public std::enable_shared_from_this<CameraComponent>
	{
	public:

		CameraComponent();
		virtual ~CameraComponent();

		void Init(std::weak_ptr<MyLib::GameObject> parent) override;
		void Start() override;
		void Update() override;
		void End() override;

		// ゲッター関数群
		float	GetFov()		const { return m_fov; }
		float	GetNear()		const { return m_near; }
		float	GetFar()		const { return m_far; }
		int		GetPriority()	const { return m_priority; }
		const Position3 GetPosition() const;
		const Position3& GetTarget() const { return m_target; }

		// セッター関数群
		void SetFov(float fov) { m_fov = fov; }
		void SetNearFar(float cameraNear, float cameraFar) { m_near = cameraNear; m_far = cameraFar; }
		void SetPriority(int priority) { m_priority = priority; }
		void SetTarget(const Vector3& target) { m_target = target; }

	private:

		float m_fov;							// 視野角 
		float m_near;							// カメラが描画する最も近い距離
		float m_far;							// カメラが描画する最も遠い距離
		int m_priority;							// カメラの優先度
		Vector3 m_target;						// カメラが見るターゲット

		std::weak_ptr<Transform> m_pTransform;	// 位置関連データ
	};

}


