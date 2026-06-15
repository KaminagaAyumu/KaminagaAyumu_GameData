using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BossAnimation : MonoBehaviour
{
    //レイヤー名
    [SerializeField] private string m_targetLayer = "Player";

    //攻撃の範囲内に入っている間はずっと呼ばれる
    private void OnTriggerStay(Collider other)
    {
        //レイヤー名がPlayerの場合に呼ばれる
        if (other.gameObject.layer == LayerMask.NameToLayer(m_targetLayer))
        {
            Debug.Log("攻撃範囲内に入りました");
        }
    }

    //Playerが範囲外に出たときに呼ばれる処理
    private void OnTriggerExit(Collider other)
    {
        if (other.gameObject.layer == LayerMask.NameToLayer(m_targetLayer))
        {
            Debug.Log("攻撃範囲外から出ました");
        }
    }
}
