using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class InGamePanelManager : MonoBehaviour
{
    //InGame中にプレイヤーが死んだらプレイヤー死亡時パネルを出現させる
    //プレイヤー死亡時パネル
    [Header("プレイヤー死亡時パネル(ウィンドウ)")]
    [SerializeField] private GameObject m_playerDeathPanel;
    [Header("インゲームUIパネル(ウィンドウ")]
    [SerializeField] private GameObject m_inGamePanel;
    [Header("カーソル")]
    [SerializeField] private GameObject m_cursor;

    //プレイヤーが死んでるかを取るためにプレイヤーの参照を得る
    [Header("プレイヤー")]
    [SerializeField] private Player m_player;

    //死亡時パネルのキャンバスグループ
    private CanvasGroup m_playerDeathCanvasGroup;
    //インゲームUIパネルのキャンバスグループ
    private CanvasGroup m_inGamePanelCanvasGroup;

    //死亡時のパネルにあるどのボタンにフォーカスを当てるか
    [SerializeField] private Button m_restartButton;

    //死亡時のパネルの初期化を行ったか
    bool m_isInitDeathPanel = false;

    private float m_selectableTime = 4.0f;//選択可能になるまでの時間

    // Start is called before the first frame update
    void Start()
    {
        //最初のパネルのアルファは0にしておく
        m_playerDeathCanvasGroup = m_playerDeathPanel.GetComponent<CanvasGroup>();
        m_playerDeathCanvasGroup.alpha = 0.0f;
        //パネルからCanvasGroupを取得
        m_inGamePanelCanvasGroup = m_inGamePanel.GetComponent<CanvasGroup>();
    }

    // Update is called once per frame
    void Update()
    {
        //プレイヤーが死んでいるかを取得する
        bool isDeadPlayer = m_player.GetHP() <= 0;
        //死亡時パネルのアルファを保持する
        float playerDeathCGAlpha = m_playerDeathCanvasGroup.alpha;

        if(isDeadPlayer)
        {
            //既にPlayerDeathPanelのアルファが既に最大なら処理を飛ばす
            if (playerDeathCGAlpha >= 1.0f) return;

            //インゲームのUIに反応しないように設定する
            m_inGamePanelCanvasGroup.interactable = false;
            m_inGamePanelCanvasGroup.blocksRaycasts = false;

            m_playerDeathCanvasGroup.alpha += 0.6f * Time.deltaTime;

            //死亡時パネルの初期化を行っていない場合だけ処理する　
            if (!m_isInitDeathPanel)
            {
                //死亡時パネルをアクティブ化して、
                //インゲームパネルのアルファを下げる
                m_playerDeathPanel.SetActive(true);
                // 最初は選択できないようにカーソルを非アクティブ化する
                m_cursor.SetActive(false);
                m_inGamePanelCanvasGroup.alpha = 0.3f;

                //初期化完了フラグを立てる
                m_isInitDeathPanel = true;

                // 選択可能になるまでの時間を待ってからカーソルをアクティブ化するコルーチンを開始する
                StartCoroutine(EnableDeathPanelCursor());
            }
        }
    }

    private IEnumerator EnableDeathPanelCursor()
    {
        yield return new WaitForSeconds(3.0f);
        // 選択可能になったらカーソルをアクティブ化する
        m_cursor.SetActive(true);
        //ボタンのフォーカスをセッティングウィンドウにする
        EventSystem eventSystem = EventSystem.current;
        eventSystem.SetSelectedGameObject(null);
        eventSystem.SetSelectedGameObject(m_restartButton.gameObject);
    }
}
