#pragma once
#include "../StateBase.h"
#include "../../../Geometry/Vector3.h"

namespace MyLib
{
	class EnemyController;
	class Animator;
	class GameObject;

	class EnemyStateDead : public StateBase<EnemyController>
	{
	public:

		EnemyStateDead();
		virtual ~EnemyStateDead();

		virtual void OnInit(EnemyController* owner) override;
		virtual void OnUpdate() override;
		virtual void OnEnd() override;

	private:
		std::weak_ptr<Animator> m_pAnimator;

		std::weak_ptr<GameObject> m_pOwnerObj;

		// ノックバックの向き
		Vector3 m_knockBackDir;
	};
}


