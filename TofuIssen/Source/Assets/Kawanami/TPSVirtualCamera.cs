////using System;
using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class TPSVirtualCamera : MonoBehaviour
{
    [Header("追跡対象の位置")]
    [SerializeField]
    // 追跡対象のTransform
    private Transform m_target;

    [Header("回転用空オブジェクトの位置")]
    [SerializeField] private Transform m_yawTransform;
    [SerializeField] private Transform m_pitchTransform;

    [Header("カメラオブジェクト")]
    [SerializeField]
    private Cinemachine.CinemachineVirtualCamera m_camera;
    [SerializeField] private CameraSwitcher m_cameraSwitcher;

    //    [Header("プレイヤー参照用オブジェクト")]
    //    [SerializeField]
    //    private Transform m_cameraTarget;

    [Header("カメラ感度")]
    [SerializeField]
    const float kSensitivity = 100f;

    [Header("通常カメラFov")]
    [SerializeField]
    private float m_normalFov = 60f;
    [Header("ロックオンカメラFov")]
    [SerializeField]
    private float m_lockOnFov = 75f;
    [Header("通常カメラ距離")]
    [SerializeField]
    private float m_normalCameraDistance = 7f;
    [Header("ロックオンカメラ距離")]
    [SerializeField]
    private float m_lockOnCameraDistance = 5f;
    [Header("通常カメラ高さ")]
    [SerializeField]
    private float m_normalCameraHeight = 1.5f;
    [Header("ロックオンカメラ高さ")]
    [SerializeField]
    private float m_lockOnCameraHeight = 3.0f;

    [Header("追従のLerp値")]
    [SerializeField]
    private float m_followLerp = 5f;

    [Header("カメラ回転のLerp値")]
    [SerializeField]
    private float m_rotateLerp = 5f;

    [Header("カメラ設定変更のLerp値")]
    [SerializeField]
    private float m_cameraSetLerp = 5f;

    [Header("カメラ当たり判定のレイヤー")]
    [SerializeField]
    private LayerMask m_cameraCollisionLayer;

    // ロックオンした対象のTransform
    private Transform m_lockOnTarget;

    // ロックオンしたい対象のタグ名
    const string kBossTag = "Enemy";
    //プレイヤーのタグ
    const string kPlayerTag = "Player";

    // カメラ距離の現在値
    private float m_cameraDistance;
    // カメラの高さ
    private float m_cameraHeight;

    // カメラの縦回転の最小値と最大値
    const float kMinPitch = -90f;
    // カメラの縦回転の最大値
    const float kMaxPitch = 90f;

    // カメラの反転設定（1か-1）
    private int m_inversionIndexX = 1;
    private int m_inversionIndexY = 1;
    // カメラの当たり判定の半径
    const float kCollisionRadius = 0.5f;

    private float m_yaw;
    private float m_pitch;
    // カメラシェイクのパワー
    private float m_shakePower;
    // カメラシェイクの時間
    private float m_shakeTime;
    // カメラの初期位置
    private Vector3 m_defaultCameraPos;

    // カメラの状態
    enum CameraState
    {
        // 通常
        normal,
        // ロックオン
        lockOn,
        //bossintro
        intro
    }

    // カメラの状態を管理する変数
    private CameraState m_cameraState = CameraState.normal;

    // Start is called before the first frame update
    void Start()
    {
        // カメラの初期回転を取得
        Vector3 rot = transform.eulerAngles;
        m_yaw = rot.y;
        m_pitch = rot.x;
        SetStartCameraRotate();
        m_camera.m_Lens.FieldOfView = m_normalFov;
        m_cameraDistance = m_normalCameraDistance;
        m_cameraHeight = m_normalCameraHeight;
        m_camera.transform.localPosition = new Vector3(0f, m_normalCameraHeight, -m_normalCameraDistance);
        m_defaultCameraPos = m_camera.transform.localPosition;
        // カメラの上下左右の操作方法を取得
        m_inversionIndexX = PlayerPrefs.GetInt("CameraInversionIndexX");
        m_inversionIndexY = PlayerPrefs.GetInt("CameraInversionIndexY");
    }

    // Update is called once per frame
    void LateUpdate()
    {
        // カメラの状態に応じた処理
        if (m_cameraState == CameraState.normal)
        {
            UpdateNormal();
        }
        else if (m_cameraState == CameraState.lockOn)
        {
            UpdateLockOn();
        }
        else if (m_cameraState == CameraState.intro)
        {
            UpdateIntro();
        }
    }

    /// <summary>
    /// ノーマル更新
    /// </summary>
    void UpdateNormal()
    {
        FollowTarget();
        RotateByStick();
        SetCameraFovAndDistance();
        UpdateShakeCamera();
        CameraCollision();

        //右スティック押し込みで切り替え
        if (Input.GetButtonDown("RightStickPush"))
        {
            StartLockOn();
        }
#if DEBUG
        if (Input.GetKeyDown("1"))
        {
            SetCameraInversionX(true);
        }
        if (Input.GetKeyDown("2"))
        {
            SetCameraInversionX(false);
        }
        if (Input.GetKeyDown("3"))
        {
            SetCameraInversionY(true);
        }
        if (Input.GetKeyDown("4"))
        {
            SetCameraInversionY(false);
        }
#endif
    }

    /// <summary>
    /// ロックオン更新
    /// </summary>
    void UpdateLockOn()
    {
        FollowTarget();
        RotateByLockOn();
        SetCameraFovAndDistance();
        UpdateShakeCamera();
        CameraCollision();

        //右スティック押し込みで切り替え
        if (Input.GetButtonDown("RightStickPush"))
        {
            EndLockOn();
        }
    }
    //イントロムービー中更新
    void UpdateIntro()
    {
        FollowTarget();
    }

    /// <summary>
    /// ターゲット追従
    /// </summary>
    void FollowTarget()
    {
        transform.position = Vector3.Lerp(transform.position, m_target.position, Time.deltaTime * m_followLerp);
    }

    /// <summary>
    /// スティック入力によるカメラ回転
    /// </summary>
    void RotateByStick()
    {
        if(m_cameraSwitcher.GetHighPriorityCamera() == CameraSwitcher.CameraType.intro)
        {
            return;
        }

        // スティックの入力を取得
        float stickX = Input.GetAxis("RightStickX");
        float stickY = Input.GetAxis("RightStickY");

        //デッドゾーン
        if (Mathf.Abs(stickX) < 0.1f)
        {
            stickX = 0f;
        }
        if (Mathf.Abs(stickY) < 0.1f)
        {
            stickY = 0f;
        }

        //カメラ感度と時間を考慮して回転量を計算
        m_yaw += stickX * kSensitivity * Time.deltaTime * m_inversionIndexX;
        m_pitch += stickY * kSensitivity * Time.deltaTime * m_inversionIndexY;

        //縦方向の回転を制限
        m_pitch = Mathf.Clamp(m_pitch, kMinPitch, kMaxPitch);
        //左右回転
        m_yawTransform.rotation = Quaternion.Euler(0f, m_yaw, 0f);
        //上下回転
        m_pitchTransform.localRotation = Quaternion.Euler(m_pitch, 0f, 0f);
    }

    /// <summary>
    /// ロックオン開始処理
    /// </summary>
    void StartLockOn()
    {
        // ロックオン対象をタグで検索
        GameObject target = GameObject.FindGameObjectWithTag(kBossTag);
        //ターゲットが見つかった場合、ロックオン対象に設定
        if (target != null)
        {
            m_lockOnTarget = target.transform;
            m_cameraState = CameraState.lockOn;
        }
    }

    //ロックオンによるカメラ回転
    void RotateByLockOn()
    {
        if (m_lockOnTarget == null)
        {
            return;
        }
        //ターゲットへのベクトル取得
        Vector3 dir = m_lockOnTarget.position - m_yawTransform.position;
        //回転量取得
        Quaternion rot = Quaternion.LookRotation(dir);
        //yawとpitch変換用
        float targetYaw =
            NormalizeAngle(rot.eulerAngles.y);
        float targetPitch =
            NormalizeAngle(rot.eulerAngles.x);

        //線形補間させる
        m_yaw = Mathf.LerpAngle(
            m_yaw,
            targetYaw,
            Time.deltaTime * m_rotateLerp);
        m_pitch = Mathf.LerpAngle(
            m_pitch,
            targetPitch,
            Time.deltaTime * m_rotateLerp);
        //左右回転
        m_yawTransform.rotation =
            Quaternion.Euler(0f, m_yaw, 0f);
        //上下回転
        m_pitchTransform.localRotation =
            Quaternion.Euler(m_pitch, 0f, 0f);
    }
    /// <summary>
    /// ロックオン終了処理
    /// </summary>
    void EndLockOn()
    {
        m_cameraState = CameraState.normal;
        m_lockOnTarget = null;
        SetCameraFovAndDistance();
    }
    /// <summary>
    /// 回転の正規化
    /// </summary>
    float NormalizeAngle(float angle)
    {
        //180度超えたら逆にする
        if (angle > 180f)
        {
            angle -= 360f;
        }
        return angle;
    }

    /// <summary>
    /// カメラの上下反転設定
    /// </summary>
    /// <param name="isInverted">trueの場合カメラの上下が反転</param>
    public void SetCameraInversionX(bool isInverted)
    {
        //上下反転
        m_inversionIndexX = isInverted ? -Mathf.Abs(m_inversionIndexX) : Mathf.Abs(m_inversionIndexX);
    }
    /// <summary>
    /// カメラの左右反転設定
    /// </summary>
    /// <param name="isInverted">trueの場合カメラの左右が反転</param>
    public void SetCameraInversionY(bool isInverted)
    {
        //左右反転
        m_inversionIndexY = isInverted ? -Mathf.Abs(m_inversionIndexY) : Mathf.Abs(m_inversionIndexY);
    }
    /// <summary>
    /// カメラの視野角設定
    /// </summary>
    void SetCameraFovAndDistance()
    {
        //カメラの状態に応じて視野角と距離を変更
        if (m_cameraState == CameraState.lockOn)
        {
            m_camera.m_Lens.FieldOfView = Mathf.Lerp(m_camera.m_Lens.FieldOfView, m_lockOnFov, Time.deltaTime * m_cameraSetLerp);
            m_cameraDistance = Mathf.Lerp(m_cameraDistance, m_lockOnCameraDistance, Time.deltaTime * m_cameraSetLerp);
            m_cameraHeight = Mathf.Lerp(m_cameraHeight, m_lockOnCameraHeight, Time.deltaTime * m_cameraSetLerp);
            m_camera.transform.localPosition = new Vector3(0f, m_cameraHeight, -m_cameraDistance);
            m_defaultCameraPos = m_camera.transform.localPosition;
        }
        else
        {
            m_camera.m_Lens.FieldOfView = Mathf.Lerp(m_camera.m_Lens.FieldOfView, m_normalFov, Time.deltaTime * m_cameraSetLerp);
            m_cameraDistance = Mathf.Lerp(m_cameraDistance, m_normalCameraDistance, Time.deltaTime * m_cameraSetLerp);
            m_cameraHeight = Mathf.Lerp(m_cameraHeight, m_normalCameraHeight, Time.deltaTime * m_cameraSetLerp);
            m_camera.transform.localPosition = new Vector3(0f, m_cameraHeight, -m_cameraDistance);
            m_defaultCameraPos = m_camera.transform.localPosition;
        }
    }
    //カメラとオブジェクトの当たり判定
    void CameraCollision()
    {
        //カメラの位置からレイを飛ばす
        Vector3 origin = m_target.position;
        //飛ばす方向
        Vector3 dir = -m_camera.transform.forward;
        //返り値を入れる変数
        RaycastHit hit;
        //レイを飛ばす距離
        float distance = m_cameraDistance;
        //レイが当たったら
        if (Physics.SphereCast(origin, kCollisionRadius, dir, out hit, distance, m_cameraCollisionLayer))
        {
            //当たった距離に変更
            distance = hit.distance;
            //カメラの位置を更新
            m_camera.transform.localPosition = new Vector3(
                m_camera.transform.localPosition.x,
                m_camera.transform.localPosition.y,
                -distance);
        }
    }
    //カメラシェイク開始
    public void StartShakeCamera(float power, float time)
    {
        if (m_camera == null)
        {
            return;
        }
        //カメラシェイクのパワーと時間を設定
        m_shakePower = power;
        m_shakeTime = time;
    }
    //カメラシェイクの更新
    void UpdateShakeCamera()
    {
        //シェイク時間が経過したら元の位置に戻す
        if (m_shakeTime > 0)
        {
            //シェイクパワーに応じてランダムな位置を生成
            Vector3 shakeOffset = UnityEngine.Random.insideUnitSphere * m_shakePower;

            //カメラの位置を更新
            m_camera.transform.localPosition = m_defaultCameraPos + shakeOffset;

            //シェイクカウントを進める
            m_shakeTime -= Time.deltaTime;
        }
        else
        {
            //カメラの位置を元に戻す
            m_camera.transform.localPosition = Vector3.Lerp(
                m_camera.transform.localPosition,
                m_defaultCameraPos,
                Time.deltaTime * m_cameraSetLerp);
            m_shakePower = 0;
            m_shakeTime = 0f;
        }
    }
    /// <summary>
    /// カメラの初期回転を設定する
    /// </summary>
    void SetStartCameraRotate()
    {
        // ボスの方向を見るためにボスを取得
        GameObject target = GameObject.FindGameObjectWithTag(kBossTag);

        //ターゲットへのベクトル取得
        Vector3 dir = target.transform.position - m_yawTransform.position;
        //回転量取得
        Quaternion rot = Quaternion.LookRotation(dir);

        //yawとpitch変換用
        float targetYaw =
            NormalizeAngle(rot.eulerAngles.y);
        float targetPitch =
            NormalizeAngle(rot.eulerAngles.x);

        //回転値を設定
        m_yaw = targetYaw;
        m_pitch = targetPitch;
      
        //左右回転
        m_yawTransform.rotation =
            Quaternion.Euler(0f, m_yaw, 0f);
        //上下回転
        m_pitchTransform.localRotation =
            Quaternion.Euler(m_pitch, 0f, 0f);
    }
    /// <summary>
    /// ロックオンしているかどうか
    /// </summary>
    /// <returns></returns>
    public bool IsLockOn()
    {
        if(m_cameraState == CameraState.normal)
        {
            return false;
        }
        else 
        {
            return true;
        }
    }
}