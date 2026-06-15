using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class UICode : MonoBehaviour
{
    [SerializeField] Image image_tens;
    [SerializeField] Image image_ones;
    public Sprite[] sprites;
    Vector3 m_Vel;
    float m_lifeTimeMax = 0;
    float m_time = 0;

    // Start is called before the first frame update
    void Start()
    {
        m_Vel = new Vector3(0, 10.0f, 0);//上方向に流れる速度
    }

    // Update is called once per frame
    void Update()
    {
        //やりたいこと
        //1.出た瞬間に少し大きくして元に戻す(当たった感が出るらしい)
        //2.少し上方向に流れる→徐々に消える
        //3.クリティカル(SetNumber()の処理が変わるため、少し面倒)

        //1.
      this.transform.position += m_Vel * Time.deltaTime ;
        //2.
        m_time += Time.deltaTime;
        float alpha = 1.0f - (m_time / m_lifeTimeMax);//割合
        Color col = new Color(1f, 1f, 1f, Mathf.Clamp(alpha,0.6f,1.0f));//0~1の範囲に収める
        image_tens.color = col;
        image_ones.color = col;


    }

    public void SetNumber(int number)
    {
        if (number < 0 || number > 99)
        {
            Debug.LogError("Number must be between 0 and 99.");
            return;
        }
        int tens = number / 10;
        int ones = number % 10;
        image_tens.sprite = sprites[tens];
        image_ones.sprite = sprites[ones];
    }
    public void SetLifeTime(float seconds)
    {
        m_lifeTimeMax = seconds;
        m_time = 0.0f;
    }
}
