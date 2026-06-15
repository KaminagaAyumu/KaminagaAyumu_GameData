using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraSwitcher : MonoBehaviour
{
    //カメラタイプ
    public enum CameraType
    {
        normal = 0,//通常カメラ
        parry = 1,//プレイヤーが呼ぶ
        intro = 2,//プレイヤーが呼ぶ
        end = 3//ボス死んでるよカメラ//ボスで呼ぶ
    }

    [Header("通常カメラ")]
    [SerializeField]
    private Cinemachine.CinemachineVirtualCamera m_normalCamera;

    [Header("パリィカメラ")]
    [SerializeField]
    private Cinemachine.CinemachineVirtualCamera m_parryCamera;

    [Header("イントロカメラ")]
    [SerializeField]
    private Cinemachine.CinemachineVirtualCamera m_introCamera;

    [Header("エンドカメラ")]
    [SerializeField]
    private Cinemachine.CinemachineVirtualCamera m_endCamera;

    public CameraType GetHighPriorityCamera()
    {
        if(m_normalCamera.Priority == 10)return CameraType.normal;
        if(m_parryCamera.Priority == 10) return CameraType.parry;
        if(m_introCamera.Priority == 10) return CameraType.intro;
        if(m_endCamera.Priority == 10) return CameraType.end;
        return CameraType.normal;
    }

    // Start is called before the first frame update
    void Start()
    {
        //通常カメラスタート
        m_normalCamera.Priority = 10;
        m_introCamera.Priority = 0;
        m_endCamera.Priority = 0;
        m_parryCamera.Priority = 0;
    }
    /// <summary>
    /// 指定されたvirtualCameraに変更する
    /// </summary>
    /// <param name="type">カメラタイプ</param>
    public void SwitchCamera(CameraType type)
    {
        m_normalCamera.Priority = 9;
        m_introCamera.Priority = 0;
        m_endCamera.Priority = 0;
        m_parryCamera.Priority = 0;

        switch (type)
        {
            case CameraType.normal:
                m_normalCamera.Priority = 10;
                break;

            case CameraType.intro:
                m_introCamera.Priority = 10;
                break;

            case CameraType.end:
                m_endCamera.Priority = 10;
                break;

            case CameraType.parry:
                m_parryCamera.Priority = 10;
                break;
        }



    }
}
