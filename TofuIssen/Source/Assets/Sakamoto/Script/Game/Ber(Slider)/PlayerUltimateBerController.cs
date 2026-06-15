using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class PlayerUltimateBerController : MonoBehaviour
{
    [Header("プレイヤーのアルティメットスライダー")]
    [SerializeField] private Image m_currentUltimateSlider;

    [Header("プレイヤー")]
    [SerializeField] private Player m_player;

    //プレイヤーのアルティメットゲージ量
    float m_ultimateGage = 0.0f;
    //最大アルティメットゲージ量
    float m_maxUltimateGage = 0.0f;

    //アルティメットゲージがどれぐらいの時間をかけて増減するか
    [Header("ゲージ増減のLerpのtの値")]
    [SerializeField] private float m_delayDuration = 0.3f;

    // Start is called before the first frame update
    void Start()
    {
        //プレイヤーのアルティメットゲージ量を取得。同時に最大アルティメットゲージ量も保持する
        m_maxUltimateGage = 100.0f;
        m_ultimateGage = m_player.GetUltGage();

        //念のためスライダーのvalueを初期化
        m_currentUltimateSlider.fillAmount = 0.0f;
    }

    // Update is called once per frame
    void Update()
    {
        //毎フレームアルティメットゲージ量を取得する
        m_ultimateGage = m_player.GetUltGage();

        //アルティメットゲージを0～1に変換する
        float rate = m_ultimateGage / m_maxUltimateGage;

        float currentUltValue = m_currentUltimateSlider.fillAmount;

        //fillAmountにLerpをかけて適用する
        float speed = (1.0f / m_delayDuration) * Time.deltaTime;

        //できた値をスライダーのfillAmountに入れる
        m_currentUltimateSlider.fillAmount = Mathf.Lerp(currentUltValue,
            rate,
            speed);
    }
}
