#pragma once
#include "../StateBase.h"

namespace MyLib
{
    class PlayerController;
    class Animator;

    class PlayerStateDead : public StateBase<PlayerController>
    {
    public:

        PlayerStateDead();
        virtual ~PlayerStateDead() = default;

        virtual void OnInit(PlayerController* owner) override;
        virtual void OnUpdate() override;
        virtual void OnEnd() override;

    private:

        std::weak_ptr<Animator> m_pAnimator;
    };
}



