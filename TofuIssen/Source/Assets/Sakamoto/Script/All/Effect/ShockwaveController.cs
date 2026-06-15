using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class ShockwaveController : MonoBehaviour
{
    //波動シェーダ
    [SerializeField] private Shader m_shockWaveShader;
    //作成したマテリアルを保持する
    private List<Material> m_materials = new List<Material>();

    //歪みが広がっていくときのタイマー管理
    //歪みを連続でかけたいときのためにListで管理する
    List<float> m_times = new List<float>();
    //アニメーションの長さ
    [SerializeField] float m_animTime;
    //最大半径
    [SerializeField] private float m_maxRadius = 100;
    //歪みの強さ最大
    [SerializeField] private float m_maxDistStr = 100;

    //使い終わった削除させるタイマーを違うリストに保持させる
    List<int> m_deleteTimer = new List<int>();

    private void OnRenderImage(RenderTexture source, RenderTexture destination)
    {
        //アニメーションが有効であればマテリアルを適用してカメラの描画を行う
        if (m_times.Count > 0)
        {
            List<RenderTexture> temps = new List<RenderTexture>();
            temps.Add(RenderTexture.GetTemporary(source.descriptor));
            Graphics.Blit(source, temps[0], m_materials[0]);
            if (m_materials.Count > 1)
            {
                for (int i = 0; i < m_materials.Count - 1; i++)
                {
                    temps.Add(RenderTexture.GetTemporary(source.descriptor));
                    Graphics.Blit(temps[i], temps[i + 1], m_materials[i + 1]);
                }
            }
            Graphics.Blit(temps[m_materials.Count - 1], destination, m_materials[m_materials.Count - 1]);
            for (int i = 0; i < temps.Count; i++)
            {
                RenderTexture.ReleaseTemporary(temps[i]);
            }
        }
        else//それ以外であればマテリアルを適用しない
        {
            Graphics.Blit(source, destination);
        }
    }

    void Awake()
    {
        
    }

    void Update()
    {
        //アニメーション中のみ処理を行う
        if (m_times.Count > 0)
        {
            //存在している時間を更新する
            for (int i = 0; i < m_times.Count; i++)
            {
                m_times[i] += Time.deltaTime;

                //アニメーションの進行度割合を計算
                float progress = m_times[i] / m_animTime;

                //時間が0の時は半径は0、m_animTimeに達したときは半径はmaxRadiusになる
                float radius = progress * m_maxRadius;

                //歪みの強さを計算する
                //時間が0の時は一番歪みが強く、animTimeに達したら一番弱くする
                float distStr = (1.0f - progress) * m_maxDistStr;

                //計算した値をマテリアルに渡す
                m_materials[i].SetFloat("_Radius", radius);
                m_materials[i].SetFloat("_DistortionStrength", distStr);
                m_materials[i].SetVector("_Center", new Vector4(0.5f, 0.5f, 0.0f, 0.0f));
                m_materials[i].SetFloat("_Width", 0.2f);

                if (m_times[i] >= m_animTime)
                {
                    //アニメーションが終わったタイマーを削除するリストに入れる
                    m_deleteTimer.Add(i);
                }
            }
            for (int i = m_deleteTimer.Count-1; i >= 0; i--)
            {
                m_times.RemoveAt(m_deleteTimer[i]);
                Destroy(m_materials[m_deleteTimer[i]]);
                m_materials.RemoveAt(m_deleteTimer[i]);
            }
            m_deleteTimer.Clear();
        }

#if UNITY_EDITOR
        //Cキーが押されたらエフェクトを有効にする
        if (Input.GetKeyDown(KeyCode.C))
        {
            OnShockwave();
        }
#endif
    }

    public void OnShockwave()
    {
        m_times.Add(0.0f);
        m_materials.Add(new Material(m_shockWaveShader));
    }
}
