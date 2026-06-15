using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

public class UIManager : MonoBehaviour
{
    //サウンドマネージャーの参照を受け取る
    [SerializeField] private SoundManager m_soundManager;
    [SerializeField] private SceneManagerKaminaga sceneManager;

    //2つの音の最初の見鳴らすため配列にする
    bool[] m_isEnter = new bool[2];
    

    // Start is called before the first frame update
    void Start()
    {
        //全てのボタンを取得
        Button[] buttons = FindObjectsOfType<Button>(true);
        //Toggleも取得
        Toggle[] toggles = FindObjectsOfType<Toggle>(true);

        //ボタンにコンポーネントを追加して参照を受け取る
        foreach (var button in buttons)
        {
            HoverSound hover = button.gameObject.AddComponent<HoverSound>();
            hover.m_soundManager = m_soundManager;
        }
        //トグルにも受け取らせる
        foreach (var toggle in toggles)
        {
            HoverSound hover = toggle.gameObject.AddComponent<HoverSound>();
            hover.m_soundManager = m_soundManager;
        }
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    //決定したとき
    public void OnDecide()
    {
        if (sceneManager.GetIsLoading() &&
            m_isEnter[0]) return;
        m_soundManager.PlaySE("Dicide");
        m_isEnter[0] = true;
    }

    public void OnGameStart()
    {
        if (sceneManager.GetIsLoading() &&
            m_isEnter[1]) return;

        m_soundManager.PlaySE("GameStart");
        m_isEnter[1] = true;
    }
}
