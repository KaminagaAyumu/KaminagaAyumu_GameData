using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class kawanamiPlayer : MonoBehaviour
{
    [Header("カメラ")]
    [SerializeField]
    private TPSVirtualCamera m_tpsCamera;

    [Header("カメラスイッチャー")]
    [SerializeField]
    private CameraSwitcher m_cameraSwitcher;

    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.Joystick1Button0))
        {
            Debug.Log("おした");
            // カメラをシェイクさせる
            m_tpsCamera.StartShakeCamera(0.1f, 1.0f);
        }

        if (Input.GetKeyDown(KeyCode.Z))
        {
            m_cameraSwitcher.SwitchCamera(CameraSwitcher.CameraType.normal);
        }

        if (Input.GetKeyDown(KeyCode.X))
        {
            m_cameraSwitcher.SwitchCamera(CameraSwitcher.CameraType.intro);
        }

        if (Input.GetKeyDown(KeyCode.C))
        {
            m_cameraSwitcher.SwitchCamera(CameraSwitcher.CameraType.end);
        }




    }
}
