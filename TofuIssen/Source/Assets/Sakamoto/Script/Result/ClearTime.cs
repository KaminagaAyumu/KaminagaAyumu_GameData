using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.UI;

public class ClearTime : MonoBehaviour
{
    //文字とスプライトの辞書を作成
    private Dictionary<string, Sprite> m_numberImage;
    //インスペクターから受け取るスプライト配列
    [Header("表示する数字画像スプライト")]
    [SerializeField] private List<Sprite> m_spriteList;
    [Header("表示するコロン画像スプライト")]
    [SerializeField] private Sprite m_colonSprite;

    //表示先のイメージを受け取る(スプライトが絵で、イメージが額縁のような感じ)
    [Header("表示先のImage(数字とコロンを入れる合計5個の額縁)")]
    [SerializeField] private List<Image> m_images;

    [Header("ベストタイム更新を表示するImage")]
    [SerializeField] private Image m_bestTimeImage;

    //クリア時間を保持する変数
    private int m_clearTime = 0;

    // Start is called before the first frame update
    void Start()
    {
        //数字と数字画像の対応表作成
        m_numberImage = new Dictionary<string, Sprite>();
        for (int i = 0; i < m_spriteList.Count; i++)
        {
            //スプライトの数字(文字)と、スプライトリストのi番目の対応表を作成
            m_numberImage.Add(i.ToString(), m_spriteList[i]);
        }

        //コロンもスプライトに登録しておく
        m_numberImage.Add(":", m_colonSprite);

        // クリアタイムをセットする
        SetClearTime((int)PlayerPrefs.GetFloat("ClearTime"));

        // ベストタイムをセットする
        SetBestTime();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void SetClearTime(int clearTime)
    {
        m_clearTime = clearTime;
    }

    private void SetBestTime()
    {
        // ベストタイムのデータが存在しなければ、ベストタイムを最大値にする
        if (!PlayerPrefs.HasKey("BestTime"))
        {
            PlayerPrefs.SetInt("BestTime", int.MaxValue);
        }

        // 現在のベストタイムを取得する
        int bestTime = PlayerPrefs.GetInt("BestTime", int.MaxValue);

        // ベストタイムが現在のクリアタイムよりも遅かったら
        if (bestTime > m_clearTime)
        {
            // 現在のクリアタイムをベストタイムとする
            PlayerPrefs.SetInt("BestTime", m_clearTime);

            // データを保存しておく
            PlayerPrefs.Save();

            // ベストタイム更新のイメージを表示する
            m_bestTimeImage.gameObject.SetActive(true);

            for(int i = 0; i < m_images.Count; i++)
            {
                // ベストタイム更新時にイメージの色を変える
                m_images[i].color = Color.yellow;
            }
        }

        //クリアタイムを受け取ったら描画する
        DrawClearTime();
    }

    private void DrawClearTime()
    {
        //まず分の部分を求める
        int minute = m_clearTime / 60;//分
        //次に秒の部分を求める
        int seconds = m_clearTime % 60;//秒

        //分の十の位を求める
        int minuteTensDigit = minute / 10;
        //分の一の位を求める
        int minuteOnesDigit = minute % 10;

        //分の十の位を求める
        int secondsTensDigit = seconds / 10;
        int secondsOnesDigit = seconds % 10;

        //それぞれのスプライトをそれぞれの額縁に渡す
        m_images[0].sprite = m_numberImage[minuteTensDigit.ToString()];
        m_images[1].sprite = m_numberImage[minuteOnesDigit.ToString()];
        m_images[2].sprite = m_numberImage[":"];
        m_images[3].sprite = m_numberImage[secondsTensDigit.ToString()];
        m_images[4].sprite = m_numberImage[secondsOnesDigit.ToString()];
    }

    [ContextMenu("クリアタイムテスト")]
    private void DebugClearTime()
    {
        SetClearTime((int)PlayerPrefs.GetFloat("ClearTime"));
    }
}
