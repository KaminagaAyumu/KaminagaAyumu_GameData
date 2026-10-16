#pragma once
#include "../StateBase.h"
#include "../../../Geometry/Vector3.h"

namespace MyLib
{
    class EnemyController;

    class EnemyStateJump : public StateBase<EnemyController>
    {
    public:

        EnemyStateJump();
        virtual ~EnemyStateJump();

        virtual void OnInit(EnemyController* owner) override;
        virtual void OnUpdate() override;
        virtual void OnEnd() override;

    private:

        /// <summary>
        /// ジャンプの流れ
        /// </summary>
        enum class JumpFlow
        {
            Default,
            JumpStart,
            Falling
        };

    private:

        // ジャンプする対象の座標
        Vector3 m_targetPos;

        // ジャンプの流れ
        JumpFlow m_flow;

        float m_defaultY;

    };
}


