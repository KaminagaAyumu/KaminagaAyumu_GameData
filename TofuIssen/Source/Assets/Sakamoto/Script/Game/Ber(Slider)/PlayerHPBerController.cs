using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.UI;

public class PlayerHPBerController : MonoBehaviour
{
    [Header("プレイヤー")]
    [SerializeField] private Player m_player;

    [Header("プレイヤーのHPスライダー")]
    [SerializeField] private Slider m_currentHPSlider;

    [Header("プレイヤーの減少HPスライダー")]
    [SerializeField] private Slider m_delayedHPSlider;

    private int m_currentHp;
    private int m_maxHp;

    //減少HPがどれぐらいの時間をかけて減るか
    private float m_delayDuration = 0.5f;

    // Start is called before the first frame update
    void Start()
    {
        //最大HPにはプレイヤーHPの初期値を入れておく
        m_maxHp = m_player.GetHP();
        //現在HPにそれを代入
        m_currentHp = m_maxHp;

        //スライダーの値を満タンにする(1にする）
        m_currentHPSlider.value = 1;
    }

    // Update is called once per frame
    void Update()
    {
        //毎プレイヤーHPを取得する
        m_currentHp = m_player.GetHP();

        //プレイヤーの現在HPが最大HPの何割にあたるかを計算
        float hpRate = (float)m_currentHp / (float)m_maxHp;

        //hpRateをスライダーのvalueに適用する
        m_currentHPSlider.value = hpRate;

        //減少HPもLerpをかけてvalueに適用する
        float decreaseSpeed = (1.0f / m_delayDuration) * Time.deltaTime;
        m_delayedHPSlider.value = Mathf.Lerp(m_delayedHPSlider.value, m_currentHPSlider.value, decreaseSpeed);
    }
}
