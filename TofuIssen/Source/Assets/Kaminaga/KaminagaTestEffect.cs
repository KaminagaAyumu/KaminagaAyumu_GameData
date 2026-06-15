using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class KaminagaTestEffect : MonoBehaviour
{

    [SerializeField] private UltEffectManager m_ultEffManager;

    void Start()
    {
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        if (Input.GetKeyDown(KeyCode.I))
        {
            Debug.Log("エフェクト発射！");
            //m_ultEffManager.CreateUltEffect();
        }
    }
}
