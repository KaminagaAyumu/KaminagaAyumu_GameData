using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HPEffectController : MonoBehaviour
{
    [SerializeField] private Material m_effectMat;
    [SerializeField] private float m_maxHP = 100.0f;
    [SerializeField] private float m_currentHP = 30.0f;
    [SerializeField] private float m_threshold = 0.3f;//30%以下でエフェクトが出るようにする
    [SerializeField] private float m_pulseSpeed = 2.0f;//エフェクトの点滅速度

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        //HPの割合を計算
        float ratio = m_currentHP / m_maxHP;

        //HP割合がm_threshold以下の場合、エフェクトを点滅させる
        if (ratio <= m_threshold)
        {
            //0から1の範囲で危険度を計算
            float dangerLevel = 1.0f - (ratio / m_threshold);
            //点滅の速さを計算
            float pulse = 0.5f + 0.5f * Mathf.Sin(Time.time * 2.0f * Mathf.PI * m_pulseSpeed);
            //マテリアルの色を変更（赤く点滅させる）
            m_effectMat.SetFloat("_intensity", dangerLevel * pulse);
        }
        else
        {
            //HPが回復したら滑らかにエフェクトを消す
            float cur = m_effectMat.GetFloat("_intensity");//現在のエフェクトの強さ
            m_effectMat.SetFloat("_intensity", Mathf.Lerp(cur, 0.0f, Time.deltaTime * 3.0f));
        }
    }
}
