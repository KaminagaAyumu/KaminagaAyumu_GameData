#pragma once
#include "../StateBase.h"

namespace MyLib
{
	class EnemyController;

	class EnemyStateFreeze : public StateBase<EnemyController>
	{
	public:

		EnemyStateFreeze();
		virtual ~EnemyStateFreeze();

		virtual void OnInit(EnemyController* owner) override;
		virtual void OnUpdate() override;
		virtual void OnEnd() override;
	};
}


