#pragma once
#include "../StateBase.h"

namespace MyLib
{
	class EnemyController;
	class Animator;

	/// <summary>
	/// プレイヤーを追いかけるときの状態
	/// </summary>
	class EnemyStateAttack : public StateBase<EnemyController>
	{
	public:

		EnemyStateAttack();
		virtual ~EnemyStateAttack();

		virtual void OnInit(EnemyController* owner) override;
		virtual void OnUpdate() override;
		virtual void OnEnd() override;

	private:

		std::weak_ptr<Animator> m_pAnimator;

	};
}
