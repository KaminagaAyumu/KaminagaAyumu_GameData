#pragma once
#include "../StateBase.h"

namespace MyLib
{
    class PlayerController;
    class SphereCollider;

    class PlayerStateGuard : public StateBase<PlayerController>
    {
    public:

        PlayerStateGuard();
        virtual ~PlayerStateGuard() = default;

        virtual void OnInit(PlayerController* owner) override;
        virtual void OnUpdate() override;
        virtual void OnEnd() override;

    private:

        std::weak_ptr<SphereCollider> m_pGuardCol;

    };
}



