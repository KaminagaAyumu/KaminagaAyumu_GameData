using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TimeCounter : MonoBehaviour
{
    private bool m_isCount = false;     // 時間をカウントするかどうか
    private float m_time = 0.0f;        // 経過時間

    /// <summary>
    /// カウントをスタートするかストップするか
    /// </summary>
    /// <param name="isCount">true : スタート false : ストップ</param>
    public void SetCounter(bool isCount)
    {
        // フラグの変更がない場合以下の処理をしない
        if(m_isCount == isCount) return;

        // 指定されたフラグの状態に変更
        m_isCount = isCount;

        // カウントを開始する場合
        if (m_isCount)
        {
            // 時間をリセットする
            m_time = 0.0f;
        }

        // 経過時間を保存する
        PlayerPrefs.SetFloat("ClearTime", m_time);
    }

    void FixedUpdate()
    {
        // カウントしない時は何もしない
        if (!m_isCount)
        {
            return;
        }

        // 経過時間を加算
        m_time += Time.fixedDeltaTime;

    }
}
