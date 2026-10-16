#pragma once
#include "../StateBase.h"

namespace MyLib
{
    class EnemyController;
    class Animator;

    class EnemyStateShot : public StateBase<EnemyController>
    {
    public:
        EnemyStateShot();
        virtual ~EnemyStateShot();

        virtual void OnInit(EnemyController* owner) override;
        virtual void OnUpdate() override;
        virtual void OnEnd() override;

    private:
        std::weak_ptr<Animator> m_pAnimator;

    };
}




