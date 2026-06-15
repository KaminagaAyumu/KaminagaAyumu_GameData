using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class DataManager : MonoBehaviour
{
    [SerializeField] private Toggle m_cameraInversionXToggle;
    [SerializeField] private Toggle m_cameraInversionYToggle;

    private void Start()
    {
#if UNITY_EDITOR
        // エディタ上ではセーブデータをリセットする
        PlayerPrefs.DeleteAll();
#endif
        // セーブデータチェック
        CheckSaveData();

        // 初期状態をセットする
        SetCameraInversionIndexX();
        SetCameraInversionIndexY();
    }

    /// <summary>
    /// セーブデータが存在するかチェックする
    /// 設定されていないデータには初期値を入れるようにする
    /// </summary>
    private void CheckSaveData()
    {
        // カメラの左右反転データを取得できなければ
        if(!PlayerPrefs.HasKey("CameraInversionIndexX"))
        {
            // デフォルトの状態を設定
            PlayerPrefs.SetInt("CameraInversionIndexX", 1);
        }
        // カメラの上下反転データを取得できなければ
        if(!PlayerPrefs.HasKey("CameraInversionIndexY"))
        {
            // デフォルトの状態を設定
            PlayerPrefs.SetInt("CameraInversionIndexY", 1);
        }
        // ベストタイムのデータを取得できなければ
        if(!PlayerPrefs.HasKey("BestTime"))
        {
            // デフォルトの状態を設定
            PlayerPrefs.SetInt("BestTime", int.MaxValue);
        }
    }

    private void SetCameraInversionIndexX()
    {
        // データからカメラの左右反転状態を得る
        int inversionIndex = PlayerPrefs.GetInt("CameraInversionIndexX", 1);
        
        // 左右反転がデフォルトの状態だったらチェックを外す
        if (inversionIndex == 1)
        {
            m_cameraInversionXToggle.isOn = false;
        }
        else
        {
            // チェックボックスのチェックをつける
            m_cameraInversionXToggle.isOn = true;
        }
    }

    private void SetCameraInversionIndexY()
    {
        // データからカメラの上下反転状態を得る
        int inversionIndex = PlayerPrefs.GetInt("CameraInversionIndexY", 1);

        // 上下反転がデフォルトの状態だったらチェックを外す
        if (inversionIndex == 1)
        {
            m_cameraInversionYToggle.isOn = false;
        }
        else
        {
            // チェックボックスのチェックをつける
            m_cameraInversionYToggle.isOn = true;
        }
    }

    public void OnInverseUpDownCamera(Toggle toggle)
    {
        if (toggle.isOn)
        {
            PlayerPrefs.SetInt("CameraInversionIndexY", -1);
        }
        else
        {
            PlayerPrefs.SetInt("CameraInversionIndexY", 1);
        }
    }

    public void OnInverseRightLeftCamera(Toggle toggle)
    {
        if (toggle.isOn)
        {
            PlayerPrefs.SetInt("CameraInversionIndexX", -1);
        }
        else
        {
            PlayerPrefs.SetInt("CameraInversionIndexX", 1);
        }
    }
}
