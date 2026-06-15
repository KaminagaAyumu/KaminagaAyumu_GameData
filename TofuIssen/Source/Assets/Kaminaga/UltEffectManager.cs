using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UltEffectManager : MonoBehaviour
{
    [Header("ボスから出てくるエフェクト")]
    [SerializeField] private GameObject m_ultEffectPrefab;

    [Header("エフェクトマネージャー")]
    [SerializeField] private EffectManager m_effectManager;

    [Header("サウンドマネージャー")]
    [SerializeField] private SoundManager m_soundManager;

    [Header("プレイヤーの位置")]
    [SerializeField] private GameObject m_playerPos;

    [Header("ウルトゲージを増やすためのクラス")]
    [SerializeField] private Player m_playerUlt;


    public System.Action<int> OnUltEffectHit;

    public void CreateUltEffect(int increaseGaugeParam)
    {
        Debug.Log("ボスULTエフェクト生成");
        GameObject eff = Instantiate(m_ultEffectPrefab, this.transform);
        eff.GetComponent<UltEffectController>().Init(m_playerPos.transform, OnUltEffectHit, increaseGaugeParam);
    }

    public void OnUltEffectHitPlayer(int increaseGaugeParam)
    {
        Debug.Log("プレイヤーULTエフェクト生成");
        m_playerUlt.IncreaseUltGage(increaseGaugeParam);
        m_effectManager.PlayEffect("UltEffect", m_playerPos.transform.position, 1.0f);
        m_soundManager.PlaySE("UltCharge");
    }


    void Start()
    {
        OnUltEffectHit += OnUltEffectHitPlayer;
    }

    void Update()
    {
        
    }
}
