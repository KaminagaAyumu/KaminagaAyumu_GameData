using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class BossScream : MonoBehaviour
{
    [Header("ボスの咆哮設定")]
    //アニメーター
    [SerializeField] private Animator m_animator;
    //咆哮のゆがみエフェクト
    [SerializeField] private ShockwaveController m_shockwaveController;
    [Header("サウンドマネージャー")]
    [SerializeField] private SoundManager m_soundManager;

    public void OnScream()
    {
        Debug.Log("OnScreamが呼ばれました！");
        if (m_shockwaveController != null)
        {
            m_shockwaveController.OnShockwave();
            //音を鳴らす
            m_soundManager.PlaySE("EnemyScream");
        }

        //親オブジェクトを経由してBossMoveを止める
        BossMove bossMove = transform.parent.GetComponentInChildren<BossMove>();
        if (bossMove != null)
        {
            bossMove.SetScreamStop(true); // 移動と回転を停止
        }
    }

    private void OnTriggerEnter(Collider other)
    {
        if (m_animator != null) m_animator.SetTrigger("Scream");
    }
}
