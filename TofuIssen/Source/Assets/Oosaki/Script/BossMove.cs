using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

public class BossMove : MonoBehaviour
{
    [Header("プレイヤーへの追従設定")]
    //ターゲットのタグ
    [SerializeField] private string m_targetTag = "Player";

    //追従するスピード
    [SerializeField] private float m_moveSpeed = 3.5f;

    //追従を停止させる距離
    [SerializeField] private float m_stopDistance = 2.0f;

    [Header("回転のスムーズさ")]
    //ボスのモデルの回転スピード
    [SerializeField] private float m_rotateSpeed = 5.0f;

    //追従するプレイヤー
    private Transform m_targetPlayer;
    //アニメーター
    private Animator m_animator;
    // Rigidbody
    private Rigidbody m_rb;
    //親オブジェクトのTransform
    private Transform m_parentTransform;

    //ボスについている攻撃スクリプトの参照
    private BossAttack m_bossAttack;

    //ボスのHP管理クラスの参照
    private BossHPController m_hpController;

    //咆哮中は停止させるフラグ
    private bool m_isScreamStopping = false;

    void Start()
    {
        m_parentTransform = transform.parent;
        m_animator = GetComponentInParent<Animator>();
        m_rb = GetComponentInParent<Rigidbody>();
        m_bossAttack = GetComponentInParent<BossAttack>();
        m_hpController = GetComponentInParent<BossHPController>();

        if (m_rb != null)
        {
            m_rb.constraints = RigidbodyConstraints.FreezeRotationX |
                               RigidbodyConstraints.FreezeRotationZ |
                               RigidbodyConstraints.FreezePositionY;

            m_rb.sleepThreshold = 0.0f;
        }
    }

    void Update()
    {
        if (m_animator == null || m_rb == null) return;

        //死亡している場合は速度ベクトルを止める
        if (m_hpController != null && m_hpController.IsDead)
        {
            m_rb.velocity = Vector3.zero;
            m_animator.SetFloat("Speed", 0f);
            m_targetPlayer = null;
            return;
        }

        //咆哮時は移動・回転もしない
        if (m_isScreamStopping)
        {
            m_rb.velocity = Vector3.zero;
            m_animator.SetFloat("Speed", 0f);
            return;
        }

        //攻撃フラグのチェック
        bool isAttackingAnim = m_animator.GetCurrentAnimatorStateInfo(0).IsTag("Attack");
        bool isAttackingScript = (m_bossAttack != null && m_bossAttack.m_isAttacking);

        if (isAttackingAnim || isAttackingScript || m_targetPlayer == null)
        {
            m_animator.SetFloat("Speed", 0f);
            return;
        }
    }
    void FixedUpdate()
    {
        if (m_animator == null || m_rb == null) return;

        if (m_hpController != null && m_hpController.IsDead)
        {
            m_rb.velocity = Vector3.zero;
            m_animator.SetFloat("Speed", 0f);
            return;
        }

        // 咆哮時は移動しない
        if (m_isScreamStopping)
        {
            m_rb.velocity = Vector3.zero;
            return;
        }

        bool isAttackingAnim = m_animator.GetCurrentAnimatorStateInfo(0).IsTag("Attack");
        bool isBossAttacking = (m_bossAttack != null && m_bossAttack.m_isAttacking);

        if (m_targetPlayer == null || isAttackingAnim || isBossAttacking)
        {
            m_rb.velocity = Vector3.zero;
            m_animator.SetFloat("Speed", 0f);
            return;
        }

        //移動・回転処理
        MoveToPlayer();
        RotateToPlayer();
    }

    private void MoveToPlayer()
    {
        float distance = Vector3.Distance(m_parentTransform.position, m_targetPlayer.position);

        if (distance > m_stopDistance)
        {
            //ターゲットとの距離をターゲットの位置+停止する距離を足して少し離れた位置で止まる
            Vector3 targetPos = m_targetPlayer.position + (m_targetPlayer.position - m_parentTransform.position).normalized * m_stopDistance;
            targetPos.y = m_parentTransform.position.y;

            Vector3 nextPos = Vector3.MoveTowards(m_parentTransform.position, targetPos, m_moveSpeed * Time.fixedDeltaTime);

            //移動制限
            float minX = -14.0f;
            float maxX = 14.0f;
            float minZ = -21.0f;
            float maxZ = 22.0f;
            float YupminX = -3.0f;
            float YupmaxX = 3.0f;
            float Yoffset = 2.0f;

            //指定した範囲を超えたら位置を調整する
            nextPos.x = Mathf.Clamp(nextPos.x, minX, maxX);
            nextPos.z = Mathf.Clamp(nextPos.z, minZ, maxZ);

            //高さも調整
            if (nextPos.x <= YupminX && nextPos.x >= YupmaxX)
            {
                nextPos.y += Yoffset;
            }

            //rigidbodyで物理計算で移動させる
            m_rb.MovePosition(nextPos);

            // アニメーションの速度を設定
            m_animator.SetFloat("Speed", m_moveSpeed);
        }
        else
        {
            m_rb.velocity = Vector3.zero;
            m_animator.SetFloat("Speed", 0f);
        }
    }

    private void RotateToPlayer()
    {
        Vector3 direction = (m_targetPlayer.position - m_parentTransform.position).normalized;
        direction.y = 0;

        if (direction != Vector3.zero)
        {
            Quaternion targetRotation = Quaternion.LookRotation(direction);
            m_parentTransform.rotation = Quaternion.Slerp(m_parentTransform.rotation, targetRotation, m_rotateSpeed * Time.deltaTime);
        }
    }

    private void OnTriggerStay(Collider other)
    {
        if (m_hpController != null && m_hpController.IsDead)
        {
            m_targetPlayer = null;
            return;
        }

        Debug.Log("プレイヤーが範囲内に入ってます");
        if (other.CompareTag(m_targetTag))
        {
            m_targetPlayer = other.transform;
        }
    }

    private void OnTriggerExit(Collider other)
    {
        if (other.CompareTag(m_targetTag))
        {
            Debug.Log("プレイヤーを見失いました");
            m_targetPlayer = null;
        }
    }

    public void SetScreamStop(bool isStop)
    {
        m_isScreamStopping = isStop;
    }
}
