using Cinemachine;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BossHPController : MonoBehaviour
{
    //ボスの体力
    [field: SerializeField] public int m_maxHp{ get; private set; } = 3;

    //現在の体力
    public int m_currentHP { get; private set; }

    public Player m_playerAttackDamage;
    [SerializeField] DamageUIJenerator m_damageUI;

    //アニメーター
    private Animator m_animator;

    public int m_damageAmount = 1;

    [Header("ダメージ演出の設定")]
    [Tooltip("ダメージモーションが発生する確率")]
    [SerializeField, Range(0f, 1f)] private float m_damageMotionChance = 0.05f;

    [Header("エフェクトマネージャー")]
    [SerializeField] private EffectManager m_effectManager;

    [Header("サウンドマネージャー")]
    [SerializeField] private SoundManager m_soundManger;

    [Header("ゲーム内の経過時間カウンタ")]
    [SerializeField] private TimeCounter m_timeCounter;

    [Header("HPのヒットエフェクト")]
    [SerializeField] private BossHpHitFlash m_hpHitEffect;

    [Header("ULTゲージを溜めるためのエフェクト管理クラス")]
    [SerializeField] private UltEffectManager m_ultEffectManager;

    [Header("死亡時の演出用カメラ")]
    [SerializeField] private CameraSwitcher m_switcher;

    //死亡しているかどうか
    public bool IsDead => m_isDead;
    private bool m_isDead = false;

    void Awake()
    {
        m_currentHP = m_maxHp;

        m_animator = GetComponentInParent<Animator>();
    }

    void Start()
    {
    }

    public void OnDamage(int damage)
    {
        //すでに死亡している場合は何もしない
        if (m_isDead) return;

        //m_isHit = true;

        //ウルトゲージを増やすエフェクトを生成
        //ウルトゲージを増やす量もここで設定
        m_ultEffectManager.CreateUltEffect(10);

        //HPを減らす
        var takeDamage = m_playerAttackDamage.GetAttackPower();
        m_currentHP -= m_playerAttackDamage.GetAttackPower();
        //プレイヤーのウルトゲージを増やす
        //m_playerAttackDamage.IncreaseUltGage(10);

        //エフェクトを再生する
        //エフェクトの出る位置をランダムな範囲にする
        float rand = Random.Range(-3.0f, 3.0f);
        Vector3 randPos = this.transform.position;
        randPos.x += rand;
        randPos.y += Mathf.Abs(rand);
        randPos.z += rand;
        m_effectManager.PlayEffect("PlayerHitAttack", randPos);

        //HPヒットエフェクトを再生
        m_hpHitEffect.StartHitFlashAnim();

        //体力のデバッグ表示
        Debug.Log("ボスにダメージ！ 残り体力:" + m_currentHP);
        //ダメージui
        m_damageUI.TakeDamage(takeDamage, this.transform.position);

        //音を鳴らす
        m_soundManger.PlaySE("PlayerAttackHit");

        //ボスの体力が0の場合は
        if (m_currentHP <= 0)
        {
            //死亡関数を実行
            Dead();
        }
        else //そうでない場合は
        {
            //ランダムでダメージアニメーションを実行
            float randomValue = Random.value;
            if (randomValue <= m_damageMotionChance)
            {
                m_animator.SetTrigger("OnDamage");
                Debug.Log("ダメージモーション再生");
            }
            else
            {
                Debug.Log("ダメージモーションをスキップ");
            }
        }
    }
    public void OnDamage()
    {
        if (m_playerAttackDamage != null)
        {
            //ダメージ処理
            OnDamage(m_playerAttackDamage.GetAttackPower());
        }
        else
        {
            OnDamage(1);
        }
    }
    /// <summary>
    /// パリー攻撃でダメージを受けたときの処理
    /// </summary>
    public void OnDamageForParry()
    {
        //すでに死亡している場合は何もしない
        if (m_isDead) return;

        //m_isHit = true;

        //ウルトゲージを増やすエフェクトを生成
        //ウルトゲージを増やす量もここで設定
        m_ultEffectManager.CreateUltEffect(40);

        //HPを減らす//若干ランダム
        int damageValue = 46 + Random.Range(0, 11); // 0から10のランダムな値を加算
        var takeDamage = damageValue;
        m_currentHP -= damageValue;
        
        //プレイヤーのウルトゲージを増やす
        //m_playerAttackDamage.IncreaseUltGage(40);

        //エフェクトを再生する
        //エフェクトの出る位置をランダムな範囲にする
        float rand = Random.Range(-3.0f, 3.0f);
        Vector3 randPos = this.transform.position;
        randPos.x += rand;
        randPos.y += Mathf.Abs(rand);
        randPos.z += rand;
        m_effectManager.PlayEffect("PlayerHitAttack", randPos);

        //体力のデバッグ表示
        Debug.Log("ボスにダメージ！ 残り体力:" + m_currentHP);
        //ダメージui
        m_damageUI.TakeDamage(takeDamage, this.transform.position);

        //HPヒットエフェクトを再生
        m_hpHitEffect.StartHitFlashAnim();

        //音を鳴らす
        m_soundManger.PlaySE("PlayerAttackHit");

        //ボスの体力が0の場合は
        if (m_currentHP <= 0)
        {
            //死亡関数を実行
            Dead();
        }
        else //そうでない場合は
        {
            //ランダムでダメージアニメーションを実行
            float randomValue = Random.value;
            if (randomValue <= m_damageMotionChance)
            {
                m_animator.SetTrigger("OnDamage");
                Debug.Log("ダメージモーション再生");
            }
            else
            {
                Debug.Log("ダメージモーションをスキップ");
            }
        }
    }

    void Dead()
    {
        m_isDead = true;

        // 時間カウンタをストップ
        m_timeCounter.SetCounter(false);

        //トリガーを発動させて死亡アニメーションを実行
        m_animator.SetTrigger("Dead");

        //死亡演出用のカメラに切り替える
        m_switcher.SwitchCamera(CameraSwitcher.CameraType.end);
    }

    private void OnTriggerEnter(Collider other)
    {
        // 自分がすでに死んでいるなら当たり判定は無視
        if (m_isDead) return;

        // ぶつかった相手のタグが "PlayerAttack" だった場合
        if (other.gameObject.CompareTag("PlayerAttack"))
        {
            Debug.Log("プレイヤーの攻撃がボスにヒット！");

            OnDamage();
        }
        if(other.gameObject.CompareTag("ParryCounter"))
        {
            Debug.Log("プレイヤーのパリィ攻撃がボスにヒット！");
            OnDamageForParry();
        }
    }
}
