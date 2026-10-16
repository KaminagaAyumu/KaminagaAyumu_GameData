#pragma once
#include "../StateBase.h"

namespace MyLib
{
    class EnemyController;

    class EnemyStateSearch : public StateBase<EnemyController>
    {
    public:

        EnemyStateSearch();
        virtual ~EnemyStateSearch();

        virtual void OnInit(EnemyController* owner) override;
        virtual void OnUpdate() override;
        virtual void OnEnd() override;

    private:
        // この状態から待機状態に移行するまでのフレーム数
        int m_searchFrame;
    };
}


