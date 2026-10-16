#pragma once
#include "Component.h"
#include "../../Geometry/Vector3.h"
#include <memory>

namespace MyLib
{
	class Transform;
	/// <summary>
	/// ターゲット化できるコンポーネント
	/// </summary>
	class Targetable : public Component
	{
	public:

		Targetable();
		virtual ~Targetable();

		void Init(std::weak_ptr<MyLib::GameObject> parent) override;
		void Start() override;
		void Update() override;
		void End() override;

		const Vector3 GetLockOnPoint() const;
		void SetLockOnOffset(const Vector3& offset) { m_lockOnOffset = offset; }

	private:
		// 座標の情報
		std::weak_ptr<Transform> m_pTransform;
		// ロックオンする座標のオフセット
		Vector3 m_lockOnOffset;
	};
}
