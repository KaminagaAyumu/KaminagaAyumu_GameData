#pragma once
#include "../StateBase.h"

namespace MyLib
{

    class EnemyController;
    class Transform;

    class EnemyStateBack : public StateBase<EnemyController>
    {
    public:
        EnemyStateBack();
        virtual ~EnemyStateBack();

        virtual void OnInit(EnemyController* owner) override;
        virtual void OnUpdate() override;
        virtual void OnEnd() override;

    private:

        std::weak_ptr<Transform> m_pPlayerPos;

        int m_backFrame;

    };
}



