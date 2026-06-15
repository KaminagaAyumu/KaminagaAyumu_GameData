using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class CountdownController : MonoBehaviour
{
    [Header("開始の際に使用するテキスト")]
    [SerializeField] private Image m_readyImage;//用意画像
    [SerializeField] private Image m_goImage;//始め！画像

    [Header("フェードにかける時間")]
    [SerializeField] private float m_fadeTime = 1;//フェードにかける時間

    [Header("ゲーム内の経過時間カウンタ")]
    [SerializeField] private TimeCounter m_timeCounter;

    //フェードタイマー
    private float m_fadeTimer;

    // Start is called before the first frame update
    void Start()
    {
        StartCoroutine(ReadyGoCoroutine());
    }

    IEnumerator FadeCoroutine(Image image, float fadeTime, bool isFadeIn)
    {
        //フェードタイマーをリセットする
        m_fadeTimer = 0.0f;

        while (m_fadeTimer < fadeTime)
        {
            //フェードタイマーを更新
            m_fadeTimer += Time.deltaTime;
            //アルファ値を計算する
            float alphaRate = m_fadeTimer / fadeTime;

            //アルファだけを変更したカラーを作成
            Color newColor = ChangeAlpha(image.color, alphaRate); ;

            //フェードインさせるなら
            if (isFadeIn)
            {
                //アルファを適用
                image.color = newColor;
            }
            else//フェードアウトなら
            {
                //アルファを逆にする
                newColor.a = 1.0f - alphaRate;
                //アルファを適用
                image.color = newColor;
            }

            //1フレーム待つ
            yield return null;
        }
    }

    IEnumerator ReadyGoCoroutine()
    {
        //Goテキストを非表示にしておく
        m_goImage.color = ChangeAlpha(m_goImage.color,0.0f);

        //Readyをフェードイン
        yield return StartCoroutine(FadeCoroutine(m_readyImage, m_fadeTime, true));

        //少し待機する
        yield return new WaitForSeconds(0.5f);

        //Readyをフェードアウトする
        yield return StartCoroutine(FadeCoroutine(m_readyImage, m_fadeTime, false));

        //Goをフェードインする
        yield return StartCoroutine(FadeCoroutine(m_goImage, m_fadeTime, true));

        // ゲーム時間のカウントを開始する
        m_timeCounter.SetCounter(true);

        //少し待機する
        yield return new WaitForSeconds(0.5f);

        //Goをフェードアウトする
        yield return StartCoroutine(FadeCoroutine(m_goImage, m_fadeTime, false));
    }

    //指定のアルファ値が入ったColorを返す
    private Color ChangeAlpha(Color color, float alpha)
    {
        Color col = new Color(color.r, color.g, color.b, alpha);
        return col;
    }
}
