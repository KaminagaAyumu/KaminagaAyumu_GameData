#pragma once
#include "../StateBase.h"
#include "../../../Geometry/Vector3.h"

namespace MyLib
{
    class PlayerController;

    class PlayerStateDash : public StateBase<PlayerController>
    {
    public:

        PlayerStateDash();
        virtual ~PlayerStateDash() = default;

        virtual void OnInit(PlayerController* owner) override;
        virtual void OnUpdate() override;
        virtual void OnEnd() override;

    private:

        // 持続フレーム
        int m_dashFrame;

        // ダッシュする速度
        float m_dashSpeed;

        // 動く向き
        Vector3 m_moveDir;
    };
}