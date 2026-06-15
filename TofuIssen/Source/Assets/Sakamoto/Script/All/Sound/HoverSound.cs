using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class HoverSound : MonoBehaviour,IPointerEnterHandler,ISelectHandler
{
    //鳴らしたい音の名前
    public string m_seName = "Select";
    //サウンドマネージャーの参照を受け取る
    public SoundManager m_soundManager;
    //シーンマネージャー
    SceneManagerKaminaga m_sceneManager;

    // Start is called before the first frame update
    void Start()
    {
        m_sceneManager = FindObjectOfType<SceneManagerKaminaga>();
        Debug.Log("m_sceneManager: " + m_sceneManager);
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void OnPointerEnter(PointerEventData eventData)
    {
        if (!m_sceneManager.GetIsLoading())
        {
            //ボタンにカーソルが乗ったときの音を再生する
            m_soundManager.PlaySE(m_seName);
        }
    }

    public void OnSelect(BaseEventData eventData)
    {
        //sceneMangerがnullなら処理を行わない
        if (m_sceneManager == null) return;

        if (!m_sceneManager.GetIsLoading())
        {
            //ボタンにカーソルが乗ったときの音を再生する
            m_soundManager.PlaySE(m_seName);
        }
    }
}
