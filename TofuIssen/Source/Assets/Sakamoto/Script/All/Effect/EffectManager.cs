using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//これを書くことでインスペクター上でエフェクトの名前と
//エフェクトのプレハブをセットできるようになる
[System.Serializable]
public class NamedEffect
{
    public string m_name;
    public GameObject m_effectPrefab;
}

public class EffectManager : MonoBehaviour
{
    //インスペクター上で名前とプレハブをセットで設定できるようにするための配列
    [SerializeField] private NamedEffect[] m_effects;

    //エフェクトの名前とプレハブを紐づけた対応表のようなもの
    //C++のstd::mapのようなものと考えるとわかりやすい
    private Dictionary<string, GameObject> m_effectDictionary;

    [Header("カメラ")]
    [SerializeField] private Camera m_camera;

    private void Awake()
    {
        //Dictionaryを初期化する
        m_effectDictionary = new Dictionary<string, GameObject>();
        foreach (var effect in m_effects)
        {
            //エフェクトの名前が空の場合は警告を出してスキップする
            if (string.IsNullOrEmpty(effect.m_name))
            {
                Debug.LogWarning("エフェクトの名前が空です。プレハブ: " + effect.m_effectPrefab);
                continue;
            }

            //エフェクトのプレハブが設定されていない場合は警告を出す   
            if (!m_effectDictionary.ContainsKey(effect.m_name))
            {
                //エフェクトの名前とプレハブをDictionaryに追加する
                m_effectDictionary.Add(effect.m_name, effect.m_effectPrefab);
            }
            else
            {
                Debug.LogWarning("エフェクトの名前が重複しています: " + effect.m_name);
            }
        }
    }

    // Update is called once per frame
    void Update()
    {
#if UNITY_EDITOR
        //Zキーが押されたら"Hit"という名前のエフェクトを再生する
        if (Input.GetKeyDown(KeyCode.Z))
        {
            PlayEffect("AvoidancePlayer", Vector3.zero);
        }
#endif
    }

    /// <summary>
    /// エフェクトを再生する
    /// </summary>
    /// <param name="effectName">エフェクトの名前</param>
    /// <param name="position">位置</param>
    /// <param name="duration">再生時間(</param>
    public void PlayEffect(string effectName, Vector3 position,float duration = -1)
    {
        //エフェクトの名前がDictionaryに存在するか確認する
        if (m_effectDictionary == null || !m_effectDictionary.ContainsKey(effectName))
        {
            //エフェクトの名前が見つからない場合はエラーログを出して終了する
            Debug.LogError("エフェクトが見つかりません: " + effectName);
            return;
        }

        //エフェクトのプレハブをインスタンス化して、指定した位置に配置する
        GameObject effect = Instantiate(m_effectDictionary[effectName], position, Quaternion.identity);


        //再生時間が渡されている(エフェクトがループする)場合は
        //その時間後に破壊する
        if (duration >= 0)
        {
            Destroy(effect, duration);
        }
        else
        {
            //再生が終わるまでループするコルーチンを呼ぶ
            StartCoroutine(WaitForEffectEnd(effect));
        }
    }

    IEnumerator WaitForEffectEnd(GameObject effect)
    {
        //エフェクトからParticleSystemを取得する
        ParticleSystem particleSystem = effect.GetComponent<ParticleSystem>();

        //もしエフェクトにParticleSystemが無かったら警告を出してDestroyする
        if (particleSystem == null)
        {
            Debug.LogError("エフェクトにParticleSystemがありません");
            Destroy(effect);
            yield break;
        }

        bool isPlaying = particleSystem.isPlaying;

        //isPlayingがfalseになったらDestroyして終了
        while (isPlaying && effect != null)
        {
            //エフェクト再生中かを取得する
            isPlaying = particleSystem.isPlaying;

            //1フレーム待機する
            //つまりループを1フレームで終わらせてしまうのではなく、
            //継続条件が満たされている間は毎フレームここがループされる
            yield return null;
        }

        //エフェクトの再生が終わったらエフェクトを破壊する
        Destroy(effect);
    }
}
