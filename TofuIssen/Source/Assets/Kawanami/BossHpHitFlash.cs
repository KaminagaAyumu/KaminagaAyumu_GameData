using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class BossHpHitFlash : MonoBehaviour
{
    [Header("ボスHPのスライダー")]
    [SerializeField]
    private Slider m_bossHpSlider;

    [Header("ボスHPのFillのRectTransform")]
    [SerializeField]
    private RectTransform m_bossHpFillRect;

    //ヒットフラッシュのアニメーター
    private Animator m_hitFlashAnim;

    // Start is called before the first frame update
    void Start()
    {
        m_hitFlashAnim = GetComponent<Animator>();
    }

    // Update is called once per frame
    void Update()
    {
        //スライダーの割合を取得
        var rate = m_bossHpSlider.value;
        //スライダーの割合とFillの幅から、positionを計算して設定
        Vector2 pos = m_bossHpSlider.transform.localPosition;
        pos.x = m_bossHpSlider.transform.localPosition.x - (m_bossHpFillRect.rect.width / 2) + m_bossHpFillRect.rect.width * rate;
        this.transform.localPosition = pos;
    }

    /// <summary>
    /// ヒットフラッシュのアニメーションを再生する
    /// </summary>
    public void StartHitFlashAnim()
    {
        this.m_hitFlashAnim.Play("HitFlash");
    }

}
