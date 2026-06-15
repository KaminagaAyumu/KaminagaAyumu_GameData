using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Audio;

//これを書くことでインスペクター上でサウンドの名前と
//サウンドをセットできるようになる
[System.Serializable]
public class NamedSound
{
    public string m_name;//名前
    public AudioClip m_audio;//音
    public bool m_isLoop;//ループするか
    public bool m_isDuplicate;//重複再生するか
    public float m_volume;//音量
}

public class SoundManager : MonoBehaviour
{
    //音の情報
    [SerializeField] private NamedSound[] m_sounds;
    //音を名前で検索するための辞書
    //Dictionaryの第二引数がNamedSoundなのは、
    //AudioClipだけではなく、ループや重複再生などの情報も再生時に参照したいから
    private Dictionary<string, NamedSound> m_soundDictionary;

    //AudioSouceを名前で検索するための辞書
    //AudioSouceに名前を付けることができる
    private Dictionary<string, AudioSource> m_audioSourceDic;

    //BGM専用スピーカー
    private AudioSource m_bgmSource;

    //音をフェードさせるときのタイマー
    private float m_fadeTimer = 0.0f;

    //AudioMixerの参照
    [Header("オーディオミキサー")]
    [SerializeField] private AudioMixer m_audioMixer;
    private AudioMixerGroup m_bgmGroup;
    private AudioMixerGroup m_seGroup; 
    private AudioMixerGroup m_voiceGroup;

    private void Awake()
    {
        //辞書を初期化
        m_soundDictionary = new Dictionary<string, NamedSound>();
        m_audioSourceDic = new Dictionary<string, AudioSource>();
        //音の名前をチェックしながらDictionaryに登録していく
        foreach (var sound in m_sounds)
        {
            //サウンドの名前が空の場合は警告を出してスキップする
            if (string.IsNullOrEmpty(sound.m_name))
            {
                Debug.LogWarning("サウンドの名前が空です。AudioClip: " + sound.m_audio);
                continue;
            }

            //音のプレハブが設定されていない場合は警告を出す   
            if (!m_soundDictionary.ContainsKey(sound.m_name))
            {
                //音の名前とプレハブをDictionaryに追加する
                m_soundDictionary.Add(sound.m_name, sound);
            }
            else
            {
                Debug.LogWarning("サウンドの名前が重複しています: " + sound.m_name);
            }
        }

        //グループとミキサー内の音の種類を紐づける
        m_bgmGroup = m_audioMixer.FindMatchingGroups("BGM")[0];
        m_seGroup = m_audioMixer.FindMatchingGroups("SE")[0];

        //AudioSourceを取得する
        m_bgmSource = gameObject.AddComponent<AudioSource>();
        //作成したAudioSourceをbgmグループに入れる
        m_bgmSource.outputAudioMixerGroup = m_bgmGroup;
    }

    /// <summary>
    /// 鳴らしたい音を名前で受け取って鳴らす
    /// </summary>
    /// <param name="soundName">音の名前</param>
    public void PlaySE(string soundName)
    {
        //渡された名前のAudioSourceが再生中であれば処理を飛ばす


        //名前がDictionaryに存在するか確認
        if (m_soundDictionary == null ||//辞書がnullの場合または、
            !m_soundDictionary.ContainsKey(soundName))//渡された名前を探して、ない場合
        {
            //サウンドの名前が見つからない場合はエラーログを出して終了する
            Debug.LogError("サウンドが見つかりません: " + soundName);
            return;
        }

        //渡された名前のサウンドがあれば辞書から取り出す
        NamedSound sound = m_soundDictionary[soundName];

        //名前に対応するAudioSourceを取り出す(なければ新しく作る)
        if (!m_audioSourceDic.ContainsKey(soundName))
        {
            //AudioSourceを取得する
            AudioSource SESource = gameObject.AddComponent<AudioSource>();

            //作成したAudioSourceをSEグループに入れる
            SESource.outputAudioMixerGroup = m_seGroup;

            //作成したAudioSourceをDictionaryに登録する
            m_audioSourceDic.Add(soundName, SESource);
        }

        //鳴らすためのAudioSourceを取得する
        AudioSource seSource = m_audioSourceDic[soundName];

        //もし重複可能なSEであれば
        if (sound.m_isDuplicate)
        {
            //その音が既になっていても止めずに音を鳴らす
            seSource.PlayOneShot(sound.m_audio, sound.m_volume);
        }
        //重複不可能なSEであれば
        else
        {
            //その音が再生中であれば処理を行わない
            if (seSource.isPlaying) return;

            //その音を止めて音を鳴らす
            seSource.clip = sound.m_audio;
            seSource.volume = sound.m_volume;
            seSource.Play();
        }
    }

    public void PlayBGM(string soundName)
    {
        //名前がDictionaryに存在するか確認
        if (m_soundDictionary == null ||//辞書がnullの場合または、
            !m_soundDictionary.ContainsKey(soundName))//渡された名前を探して、ない場合
        {
            //サウンドの名前が見つからない場合はエラーログを出して終了する
            Debug.LogError("サウンドが見つかりません: " + soundName);
            return;
        }

        //渡された名前のサウンドがあれば辞書から取り出す
        NamedSound sound = m_soundDictionary[soundName];

        //もし前のBGMがなっていたら止める
        if (m_bgmSource.isPlaying) m_bgmSource.Stop();

        //音を鳴らす
        m_bgmSource.clip = sound.m_audio;//音
        m_bgmSource.volume = sound.m_volume;//音量
        m_bgmSource.loop = sound.m_isLoop;//ループするか
        m_bgmSource.Play();//再生
    }

    public void StopBGM()
    {
        //もし前のBGMがなっていたら止める
        if (m_bgmSource.isPlaying) m_bgmSource.Stop();
    }

    public void StopSE(string soundName)
    {
        //もし渡された名前がDictionaryにないなら処理を飛ばす
        if (!m_audioSourceDic.ContainsKey(soundName)) return;

        //鳴らすためのAudioSourceを取得する
        AudioSource seSource = m_audioSourceDic[soundName];

        //もしそれが再生していない音なら処理をスキップする
        if (!seSource.isPlaying) return;

        seSource.Stop();
    }

    IEnumerator FadeCoroutine(AudioSource audio, bool isFadeIn, float volume, float fadeTime = 1.0f)
    {
        //タイマーのリセット
        m_fadeTimer = 0.0f;

        while (m_fadeTimer < fadeTime)
        {
            //フェードタイマーを更新
            m_fadeTimer += Time.deltaTime;
            //アルファ値を計算する
            float alphaRate = m_fadeTimer / fadeTime * volume;

            //フェードインさせる場合
            if (isFadeIn)
            {
                //音量を適用
                audio.volume = alphaRate;
            }
            //フェードアウトさせる場合
            else
            {
                //アルファを逆にする
                audio.volume = (1.0f - alphaRate) * volume;
            }

            //1フレーム待つ
            yield return null;
        }

        //音量が0になった場合、音を止める
        if (audio.volume <= 0.0f) StopBGM();
    }

    public void FadeInBGM(string soundName, float fadeTime = 1.0f)
    {
        //名前がDictionaryに存在するか確認
        if (m_soundDictionary == null ||//辞書がnullの場合または、
            !m_soundDictionary.ContainsKey(soundName))//渡された名前を探して、ない場合
        {
            //サウンドの名前が見つからない場合はエラーログを出して終了する
            Debug.LogError("サウンドが見つかりません: " + soundName);
            return;
        }

        //渡された名前のサウンドがあれば辞書から取り出す
        NamedSound sound = m_soundDictionary[soundName];

        //もし前のBGMがなっていたら止める
        if (m_bgmSource.isPlaying) StopBGM();

        //音を鳴らす
        m_bgmSource.clip = sound.m_audio;//音
        m_bgmSource.volume = 0.0f;//音量
        m_bgmSource.loop = sound.m_isLoop;//ループするか
        m_bgmSource.Play();//再生

        //今回鳴らしたい音量を取得
        float volume = sound.m_volume;
        //BGMをフェードさせる
        StartCoroutine(FadeCoroutine(m_bgmSource, true, volume, fadeTime));
    }

    public void FadeOutBGM(float fadeTime = 1.0f)
    {
        //もしそれが再生していない音なら処理をスキップする
        if (!m_bgmSource.isPlaying) return;
        //BGMをフェードさせる
        StartCoroutine(FadeCoroutine(m_bgmSource, false, m_bgmSource.volume, fadeTime));
    }

    [ContextMenu("BGMテスト")]
    private void DebugClearTime()
    {
        FadeInBGM("InGameBGM");
    }

    public void SetVolume(string volumeName,float volume)
    {
        Debug.Log(volumeName + ":" + volume);

        //volumeをデシベルに変換する
        float dB = FloatToDB(volume);

        //渡されたボリュームをデシベルに変換してMasterVolumeに入れる
        m_audioMixer.SetFloat(volumeName, dB);
    }

    /// <summary>
    /// float(volume)をデシベルに変換する
    /// </summary>
    private float FloatToDB(float value)
    {
        //デシベルの計算
        float dB = 0.0f;
        dB = Mathf.Log10(Mathf.Max(0.0001f,value)) * 20;
        //デシベル値を返す
        return dB;
    }
}
