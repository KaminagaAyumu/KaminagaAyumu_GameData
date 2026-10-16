#pragma once
#include "../StateBase.h"

namespace MyLib
{
    class EnemyController;

    /// <summary>
    /// 敵の待機状態
    /// </summary>
    class EnemyStateIdle : public StateBase<EnemyController>
    {
    public:

        EnemyStateIdle();
        virtual ~EnemyStateIdle();

        virtual void OnInit(EnemyController* owner) override;
        virtual void OnUpdate() override;
        virtual void OnEnd() override;

    };
}


