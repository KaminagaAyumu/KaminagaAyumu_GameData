#pragma once
#include "../StateBase.h"

namespace MyLib
{

	class EnemyController;
	class Transform;

	class EnemyStateRush : public StateBase<EnemyController>
	{
	public:

		EnemyStateRush();
		virtual ~EnemyStateRush();

		virtual void OnInit(EnemyController* owner) override;
		virtual void OnUpdate() override;
		virtual void OnEnd() override;

	private:

		std::weak_ptr<Transform> m_pPlayerPos;

		// 突進を行うフレーム数
		int m_rushFrame;

	};
}
