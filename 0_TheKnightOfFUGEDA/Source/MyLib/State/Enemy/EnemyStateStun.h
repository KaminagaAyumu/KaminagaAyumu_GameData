#pragma once
#include "../StateBase.h"

namespace MyLib
{

    class EnemyController;

    class EnemyStateStun : public StateBase<EnemyController>
    {
    public:

        EnemyStateStun();
        virtual ~EnemyStateStun();

        virtual void OnInit(EnemyController* owner) override;
        virtual void OnUpdate() override;
        virtual void OnEnd() override;

    private:

        // この状態のフレーム数
        int m_stunFrame;

    };

}
