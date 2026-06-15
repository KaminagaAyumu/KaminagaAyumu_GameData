using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class PanelManager : MonoBehaviour
{
    //タイトルパネル
    [SerializeField] private GameObject m_titlePanel;
    //セッティングパネル
    [SerializeField] private GameObject m_settingPanel;
    //本当にゲームをやめるかパネル
    [SerializeField] private GameObject m_isEndPanel;

    //タイトルのキャンバスグループ
    private CanvasGroup m_titleCanvasGroup;
    //セッティングのキャンバスグループ
    private CanvasGroup m_settingCanvasGroup;

    //各ウィンドウに遷移したときのフォーカスを当てるボタン
    [SerializeField] private Button m_settingButton;
    [SerializeField] private Button m_masterVolButton;
    [SerializeField] private Button m_noButton;

    //サウンドマネージャー
    [SerializeField] private SoundManager m_soundManager;

    //セッティングマネージャー
    [Header("UIセッティングマネージャー")]
    [SerializeField] private SettingUIManager m_settingUIManager;

    // Start is called before the first frame update
    void Awake()
    {
        //初期状態はタイトルウィンドウが出ているのでタイトルパネルのみ
        //trueにする
        m_titlePanel.SetActive(true);
        m_settingPanel.SetActive(false);
        m_isEndPanel.SetActive(false);

        //タイトルパネルとセッティングパネルについている
        //CanvasGroupコンポーネントを取得する
        m_titleCanvasGroup = m_titlePanel.GetComponent<CanvasGroup>();
        m_settingCanvasGroup = m_settingPanel.GetComponent<CanvasGroup>();
    }

    // Update is called once per frame
    void Update()
    {
        //設定画面でBボタンが押されたらOnCloseSettingを呼ぶ
        if(m_settingPanel.activeSelf)
        {
            if (Input.GetButtonDown("Cancel") ||
            Input.GetKeyDown(KeyCode.Escape))
            {
                OnCloseSetting();
            }
        }
        if (m_isEndPanel.activeSelf)
        {
            if (Input.GetButtonDown("Cancel") ||
            Input.GetKeyDown(KeyCode.Escape))
            {
                //いいえを押したときの処理を呼ぶ
                OnPushNotGameEnd();
            }    
        }
    }

    public void OnPushSetting()
    {
        //セッティングウィンドウを出現させる
        m_settingPanel.SetActive(true);
        //タイトル画面のボタンUIに反応しないように設定する
        m_titleCanvasGroup.interactable = false;
        m_titleCanvasGroup.blocksRaycasts = false;
        //タイトル画面のUIのアルファを下げる
        m_titleCanvasGroup.alpha = 0.0f;

        //ボタンのフォーカスをセッティングウィンドウにする
        EventSystem eventSystem = EventSystem.current;
        eventSystem.SetSelectedGameObject(null);
        eventSystem.SetSelectedGameObject(m_masterVolButton.gameObject);

        //セッティング中のEventSystemのNavigationを無効にする
        m_settingUIManager.OnOpenSetting();
    }

    public void OnCloseSetting()
    {
        //セッティングウィンドウを非アクティブ化させる
        m_settingPanel.SetActive(false);
        //タイトル画面のボタンUIを反応するように変更する
        m_titleCanvasGroup.interactable = true;
        m_titleCanvasGroup.blocksRaycasts = true;
        //タイトル画面のUIのアルファを戻す
        m_titleCanvasGroup.alpha = 1.0f;

        //ボタンのフォーカスをタイトルウィンドウに戻す
        EventSystem eventSystem = EventSystem.current;
        eventSystem.SetSelectedGameObject(null);
        eventSystem.SetSelectedGameObject(m_settingButton.gameObject);

        //キャンセル音を再生
        m_soundManager.PlaySE("Cancel");

        //セッティング中のEventSystemのNavigationを有効にする
        m_settingUIManager.OnCloseSetting();
    }

    public void OnEndGame()
    {
        //本当にやめるかパネルをアクティブ化する
        m_isEndPanel.SetActive(true);

        //タイトル画面のボタンUIが反応しないようにする
        m_titleCanvasGroup.interactable = false;
        m_titleCanvasGroup.blocksRaycasts = false;
        //タイトル画面のUIのアルファを下げる
        m_titleCanvasGroup.alpha = 0.06f;

        //ボタンのフォーカスを本ウィンドウに切り替える
        EventSystem eventSystem = EventSystem.current;
        eventSystem.SetSelectedGameObject(null);
        eventSystem.SetSelectedGameObject(m_noButton.gameObject);
    }

    /// <summary>
    /// ゲーム終了しますか→いいえを押したときの処理
    /// </summary>
    public void OnPushNotGameEnd()
    {
        //本当にやめるかパネルをアクティブ化する
        m_isEndPanel.SetActive(false);

        //タイトル画面のボタンUIを反応するように変更する
        m_titleCanvasGroup.interactable = true;
        m_titleCanvasGroup.blocksRaycasts = true;
        //タイトル画面のUIのアルファを戻す
        m_titleCanvasGroup.alpha = 1.0f;

        //ボタンのフォーカスをタイトルウィンドウに戻す
        EventSystem eventSystem = EventSystem.current;
        eventSystem.SetSelectedGameObject(null);
        eventSystem.SetSelectedGameObject(m_settingButton.gameObject);

        //キャンセル音を再生
        m_soundManager.PlaySE("Cancel");
    }

    public void ChangeMasterVolume(float volume)
    {
        //MasterVolumeを変更する
        m_soundManager.SetVolume("MasterVolume", volume);
    }
    public void ChangeBGMVolume(float volume)
    {
        //MasterVolumeを変更する
        m_soundManager.SetVolume("BGMVolume", volume);
    }
    public void ChangeSEVolume(float volume)
    {
        //MasterVolumeを変更する
        m_soundManager.SetVolume("SEVolume", volume);
    }
    public void ChangeVoiceVolume(float volume)
    {
        //MasterVolumeを変更する
        m_soundManager.SetVolume("VoiceVolume", volume);
    }
}
