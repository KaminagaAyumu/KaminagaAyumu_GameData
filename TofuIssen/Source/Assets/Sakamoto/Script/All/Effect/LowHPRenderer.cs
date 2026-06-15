using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

//カメラにタッチして使う
//OnRenderImageで、ポストエフェクトを画面全体に適用する
[RequireComponent(typeof(Camera))]
public class LowHPRenderer : MonoBehaviour
{
    [Header("シェーダーマテリアル")]
    [SerializeField] private Material m_material;//LowHPMatをここに入れる

    [Header("プレイヤー")]
    [SerializeField] private Player m_player;//プレイヤー
    private float m_maxHP;//最大HP
    private float m_currentHP;//現在HP
    [SerializeField] private float m_threshold = 0.3f;//30%以下でエフェクトが出るようにする
    [SerializeField] private float m_pulseSpeed = 2.0f;//エフェクトの点滅速度

    //カメラが描画し終わった直後に自動で呼ばれる
    private void OnRenderImage(RenderTexture source, RenderTexture destination)
    {
        if (m_material == null)
        {
            //マテリアル未設定の場合はそのまま描画
            Graphics.Blit(source, destination);
            return;
        }

        //sorce(カメラの描画結果)にシェーダをかけてdestinationに描画する
        Graphics.Blit(source, destination, m_material);
    }

    // Start is called before the first frame update
    void Start()
    {
        m_maxHP = m_player.GetHP();
        m_currentHP = m_player.GetHP();

        //エフェクトの強さを初期化
        m_material.SetFloat("_intensity", 0.0f);
    }

    // Update is called once per frame
    void Update()
    {
        //プレイヤーのHPをマイフレーム取得する
        m_currentHP = m_player.GetHP();

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
            m_material.SetFloat("_intensity", dangerLevel * pulse);
        }
        else
        {
            //HPが回復したら滑らかにエフェクトを消す
            float cur = m_material.GetFloat("_intensity");//現在のエフェクトの強さ
            m_material.SetFloat("_intensity", Mathf.Lerp(cur, 0.0f, Time.deltaTime * 3.0f));
        }
    }
}
