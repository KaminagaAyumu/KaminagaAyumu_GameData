using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BossAttackSensor : MonoBehaviour
{
    //親オブジェクトのボスの攻撃スクリプト
    [SerializeField] private BossAttack m_bossAttack;

    //タグ
    [SerializeField] private string m_targetTag = "Player";

    private void OnTriggerEnter(Collider other)
    {
        if (m_bossAttack == null) return;

        //タグで比較
        if (other.CompareTag(m_targetTag))
        {
            //プレイヤーが入ってきたことを通知
            m_bossAttack.OnPlayerEnter();
        }
    }

    private void OnTriggerExit(Collider other)
    {
        if (m_bossAttack == null) return;

        //タグで判定
        if (other.CompareTag(m_targetTag))
        {
            Debug.Log("攻撃範囲から出ました");
            //プレイヤーが出たことを通知
            m_bossAttack.OnPlayerExit();
        }
    }
}
