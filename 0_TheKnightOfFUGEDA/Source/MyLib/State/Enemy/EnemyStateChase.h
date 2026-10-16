#pragma once
#include "../StateBase.h"

namespace MyLib
{
	class EnemyController;
	class Transform;

	/// <summary>
	/// プレイヤーを追いかけるときの状態
	/// </summary>
	class EnemyStateChase : public StateBase<EnemyController>
	{
	public:

		EnemyStateChase();
		virtual ~EnemyStateChase();

		virtual void OnInit(EnemyController* owner) override;
		virtual void OnUpdate() override;
		virtual void OnEnd() override;

	private:

		std::weak_ptr<Transform> m_pPlayerPos;

		int m_attackFrame;

	};
}

