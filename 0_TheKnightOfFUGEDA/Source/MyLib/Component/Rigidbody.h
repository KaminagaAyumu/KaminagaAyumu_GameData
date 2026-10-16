#pragma once
#include "Component.h"
#include "../../Geometry/Vector3.h"
#include <memory>

namespace MyLib
{
	/// <summary>
	/// 挙動のタイプ
	/// </summary>
	enum class BodyType : uint8_t
	{
		Dynamic,	// 動的
		Static		// 静的
	};

	class Transform;
	class Rigidbody : public Component
	{
	public:
		Rigidbody();
		virtual ~Rigidbody();

		virtual void Init(std::weak_ptr<MyLib::GameObject> parent) override;
		virtual void Start() override;
		virtual void Update() override;
		virtual void End() override;

		void SetVelocity(const Vector3& vel) { m_velocity = vel; }
		const Vector3& GetVelocity() const { return m_velocity; }

		void SetNextPos(const Vector3& nextPos) { m_nextPos = nextPos; }
		const Vector3& GetNextPos() const { return m_nextPos; }

		void SetMass(float mass) { m_mass = mass; }
		float GetMass() const { return m_mass; }

		void SetBodyType(BodyType type) { m_bodyType = type; }
		BodyType GetBodyType() const { return m_bodyType; }

		void SetIsGravity(bool isGravity) { m_isGravity = isGravity; }
		bool IsGravity() const { return m_isGravity; }

		void SetIsApplyDirection(bool isApplyDirection) { m_isApplyDirection = isApplyDirection; }
		bool IsApplyDirection() const { return m_isApplyDirection; }

	private:
		std::weak_ptr<Transform> m_pTransform;	// 座標情報
		Vector3 m_velocity;						// 速度
		Vector3 m_nextPos;						// 未来の座標
		float m_mass;							// 質量
		BodyType m_bodyType;					// 挙動タイプ
		bool m_isGravity;						// 重力を使用するかのフラグ
		bool m_isApplyDirection;				// 向きを変えるかのフラグ
	};
}



