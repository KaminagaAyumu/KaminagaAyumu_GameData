#pragma once
#include "../StateBase.h"

namespace MyLib
{
    class EnemyController;
    class Transform;

    /// <summary>
    /// プレイヤーを発見したときの状態
    /// </summary>
    class EnemyStateDetect : public StateBase<EnemyController>
    {
    public:

        EnemyStateDetect();
        virtual ~EnemyStateDetect();

        virtual void OnInit(EnemyController* owner) override;
        virtual void OnUpdate() override;
        virtual void OnEnd() override;

    private:

        int m_attackFrame;

        std::weak_ptr<Transform> m_pPlayerPos;

    };
}


