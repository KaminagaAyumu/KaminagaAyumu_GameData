using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class BossAnimationBridge : MonoBehaviour
{
    private BossAttack m_bossAttack;

    private BossScream m_scream;

    private BossHPController m_hpController;

    //各攻撃エフェクトの生成位置
    [SerializeField] public float m_biteAttackEffectForwardRange = 5.0f;
    [SerializeField] public float m_clawAttackEffectForwardRange = 5.0f;
    [SerializeField] public float m_clawAttackEffectUpRange = 2.0f;
    [SerializeField] public float m_clawAttackEffectRightRange = 0.0f;

    //ボスのコントローラー
    [SerializeField] private BossController m_bossController;

    //各攻撃のエフェクト
    [SerializeField] EffectManager m_bossAttackEffectManager;

    void Start()
    {
        // 子オブジェクトにあるBossAttackを探して参照する
        m_bossAttack = GetComponentInChildren<BossAttack>();
        m_scream = GetComponentInChildren<BossScream>();

        m_hpController = GetComponentInParent<BossHPController>();

        if (m_bossController == null)
        {
            m_bossController = Object.FindFirstObjectByType<BossController>();
        }
    }

    //アニメーションイベント「Bite」を受け取る
    public void Bite()
    {
        if (m_bossAttack != null) m_bossAttack.Bite();

        //エフェクト生成
        m_bossAttackEffectManager.PlayEffect("BossBiteAttack",
            transform.position + transform.forward * m_biteAttackEffectForwardRange + transform.up * 2.0f);
    }

    //アニメーションイベント「Claw」を受け取る
    public void Claw()
    {
        if (m_bossAttack != null) m_bossAttack.Claw();

        Vector3 effectPosition = transform.position
        + transform.forward * m_clawAttackEffectForwardRange
        + transform.up * m_clawAttackEffectUpRange
        + transform.right * m_clawAttackEffectRightRange;

        //エフェクト生成
        m_bossAttackEffectManager.PlayEffect("BossClawAttack",effectPosition);
    }

    //アニメーションイベント「JumpAttack」を受け取る
    public void JumpAttack()
    {
        if (m_bossAttack != null) m_bossAttack.JumpAttack();
        //エフェクト生成
        m_bossAttackEffectManager.PlayEffect("BossJumpAttack", transform.position);
    }

    public void OnScream()
    {
        if (m_scream != null) m_scream.OnScream();
    }
    public void EndScream()
    {
        BossMove bossMove = GetComponentInChildren<BossMove>();
        if (bossMove == null)
        {
            bossMove = transform.parent.GetComponentInChildren<BossMove>();
        }

        if (bossMove != null)
        {
            bossMove.SetScreamStop(false); //移動と回転を戻す
        }
    }

    public void OnDead()
    {
        Debug.Log("OnDeadが呼ばれました！");
        m_bossController.DestroyBoss();
    }
}
