using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BossController : MonoBehaviour
{
    [SerializeField] private GameObject m_bossObject;

    [Header("シーンマネージャー")]
    [SerializeField] private SceneManagerKaminaga m_sceneManager;

    //ボスの削除
    public void DestroyBoss()
    {
        // リザルトシーンに遷移する
        m_sceneManager.LoadScene((int)SceneName.Result);
    }
}
