using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HpHitFlashTest : MonoBehaviour
{
    [Header("ボスHPヒットフラッシュ")]
    [SerializeField]
    private BossHpHitFlash m_bossHpHitFlash;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.JoystickButton3))
        {
            m_bossHpHitFlash.StartHitFlashAnim();
        }
    }
}
