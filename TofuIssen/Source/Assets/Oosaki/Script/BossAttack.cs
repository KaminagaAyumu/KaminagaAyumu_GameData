using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class BossAttack : MonoBehaviour
{
    public enum AttackType
    {
        None,
        Bite,
        Claw,
        JumpAttack
    }


    //外部から攻撃しているかどうかを読み取るための変数
    public bool m_isAttacking { get; private set; }

    //各攻撃のダメージ取得
    [field: SerializeField] public int m_biteDamage { get; private set; } = 10;
    [field: SerializeField] public int m_clawDamage { get; private set; } = 15;
    [field: SerializeField] public int m_jumpAttackDamage { get; private set; } = 20;
    private bool m_isPlayerInArea = false;

    //ボスの設定
    [Header("ボスの全般設定")]
    //アニメーター
    [SerializeField] private Animator m_animator;              
    //敵のレイヤー
    public LayerMask m_enemyLayer;
    //球体が消えるまでの時間
    public float m_duration = 0.2f;

    public AttackType m_attackType=AttackType.None;
    public AttackType CurrentAttackType => m_attackType;

    public int m_bossAttackDamage = 0;

    [Header("自動攻撃の設定")]
    //次の攻撃までの時間
    public float m_attackInterval = 2.0f;
    //攻撃タイマー
    private float m_attackTimer = 0f;

    [Header("噛みつき攻撃の設定")]
    //攻撃範囲
    public float m_biteRange = 1.5f;
    //攻撃判定サイズの半径
    public float m_biteRadius = 1.0f;
    //生成するプレハブ
    public GameObject m_bitePrefab;

    [Header("ひっかき攻撃の設定")]
    //攻撃範囲
    public float m_clawRange = 2.0f;
    //攻撃判定サイズの半径
    public Vector3 m_clawBoxSize = new Vector3(3.0f, 1.0f, 1.0f);
    //生成するプレハブ
    public GameObject m_clawPrefab;

    [Header("ジャンプ攻撃の設定")]
    //攻撃範囲
    public float m_jumpAttackRange = 1.5f;
    //攻撃判定サイズの半径
    public float m_jumpAttackRadius = 1.0f;
    //生成するプレハブ
    public GameObject m_jumpAttackPrefab;

    [Header("サウンドマネージャー")]
    [SerializeField] private SoundManager m_soundManager;

    void Update()
    {
        //プレイヤーが範囲内にいる、かつ攻撃中でない時だけ攻撃タイマーを進める
        if (m_attackTimer > 0)
        {
            m_attackTimer -= Time.deltaTime;
        }

        if (m_isPlayerInArea && m_attackTimer <= 0 && !m_isAttacking)
        {
            RandomAttackAction();
        }

    }

    //プレイヤーが攻撃範囲に入り続けてると呼ばれつづける関数
    public void OnPlayerEnter()
    {
        m_isPlayerInArea = true;
    }

    //プレイヤーが攻撃範囲から出たときにセンサーから呼ばれる関数
    public void OnPlayerExit()
    {
        m_isPlayerInArea = false;
    }

    private void RandomAttackAction()
    {
        if (m_isAttacking) return;
        m_isAttacking = true;

        //攻撃をランダムな値から選ぶ
        int randAttack = Random.Range(0, 3);
        //randAttack = 2; //ジャンプ攻撃のテストのため、ランダム値を2に固定

        //攻撃モーションの硬直時間
        float totalAttackTime = 2.5f;

        if (randAttack == 0)
        {
            //噛みつきアニメーションを設定
            if (m_animator != null) m_animator.SetTrigger("Bite");
        }
        else if (randAttack == 1)
        {
            //ひっかきアニメーションを設定
            if (m_animator != null) m_animator.SetTrigger("Claw");
        }
        else
        {
            //ジャンプ攻撃を設定
            if (m_animator != null) m_animator.SetTrigger("JumpAttack");

            //ジャンプ攻撃時はほかの攻撃時間よりも攻撃の硬直時間を伸ばす
            totalAttackTime = totalAttackTime + 1.0f;
        }

        StartCoroutine(AttackRoutine(totalAttackTime));
    }

    private IEnumerator AttackRoutine(float totalAttackTime)
    {
        //2.5秒待つ
        yield return new WaitForSeconds(totalAttackTime);

        //攻撃モーション終了後に解除
        m_isAttacking = false;

        //攻撃が終わった時点でプレイヤーが「まだ範囲内にいたら」次のタイマーをセット
        //範囲外に出ていればタイマーは0のままになり、次回入った瞬間に即攻撃
        if (m_isPlayerInArea)
        {
            m_attackTimer = m_attackInterval;
        }
        else
        {
            m_attackTimer = 0f;
        }
    }

    public void SetAttackType()
    {
        if(m_attackType==AttackType.Bite)
        {
            m_bossAttackDamage = m_biteDamage;
        }
        if(m_attackType == AttackType.Claw)
        {
            m_bossAttackDamage = m_clawDamage;
        }
        else
        {
            m_bossAttackDamage = m_jumpAttackDamage;
        }
    }
    public int GetBossAttackDamage()
    {
        SetAttackType();
        return m_bossAttackDamage;
    }

    public void Bite()
    {
        Debug.Log("Bite関数が呼ばれました！", this);

        m_attackType = AttackType.Bite;

        //判定位置の計算
        Vector3 attackColPos = transform.position + transform.forward * m_biteRange + (Vector3.up * 2.0f);

        //生成
        if (m_bitePrefab != null)
        {
            GameObject visualEffect = Instantiate(m_bitePrefab, attackColPos, Quaternion.identity);
            float scale = m_biteRadius * 2f;
            visualEffect.transform.localScale = new Vector3(scale, scale, scale);
            Destroy(visualEffect, m_duration);
        }

        //音を鳴らす
        m_soundManager.PlaySE("EnemyBiteAttack");

        //当たり判定
        Collider[] hitObject = Physics.OverlapSphere(attackColPos, m_biteRadius, m_enemyLayer);

        foreach (Collider obj in hitObject)
        {
            Debug.Log("Bite" + obj.name + " に当たった！");
        }
    }

    public void Claw()
    {
        //攻撃の種類を設定
        m_attackType = AttackType.Claw;

        Vector3 attackColPos = transform.position + transform.forward * m_clawRange + (Vector3.up * 3.0f);

        //生成
        if (m_clawPrefab != null)
        {
            GameObject visualEffect = Instantiate(m_clawPrefab, attackColPos, transform.rotation);

            visualEffect.transform.localScale = m_clawBoxSize;

            Destroy(visualEffect, m_duration);
        }

        //音を鳴らす
        m_soundManager.PlaySE("EnemyClawAttack");

        Vector3 halfBoxSize= m_clawBoxSize / 2.0f;

        //当たり判定
        Collider[] hitObject = Physics.OverlapBox(attackColPos, halfBoxSize, transform.rotation, m_enemyLayer);

        foreach (Collider obj in hitObject)
        {
            Debug.Log("Claw" + obj.name + " に当たった！");
        }
    }

    public void JumpAttack()
    {
        //攻撃の種類を設定
        m_attackType = AttackType.JumpAttack;

        Vector3 attackColPos = transform.position + transform.forward * m_jumpAttackRange + (Vector3.up * 3.0f);

        //生成
        if (m_jumpAttackPrefab != null)
        {
            GameObject visualEffect = Instantiate(m_jumpAttackPrefab, attackColPos, Quaternion.identity);
            float scale = m_jumpAttackRadius * 2f;
            visualEffect.transform.localScale = new Vector3(scale, scale, scale);
            Destroy(visualEffect, m_duration);
        }

        //音を鳴らす
        m_soundManager.PlaySE("EnemyJumpAttack");

        //当たり判定
        Collider[] hitObject = Physics.OverlapSphere(attackColPos, m_jumpAttackRadius, m_enemyLayer);

        foreach (Collider obj in hitObject)
        {
            Debug.Log("JumpAttack" + obj.name + " に当たった！");
        }
    }
}
