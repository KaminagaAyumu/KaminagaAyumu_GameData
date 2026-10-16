#pragma once
#include "../StateBase.h"

namespace MyLib
{
    class PlayerController;
    class PlayerStateDie : public StateBase<PlayerController>
    {
    public:

        PlayerStateDie();
        virtual ~PlayerStateDie() = default;

        virtual void OnInit(PlayerController* owner) override;
        virtual void OnUpdate() override;
        virtual void OnEnd() override;
    };
}



