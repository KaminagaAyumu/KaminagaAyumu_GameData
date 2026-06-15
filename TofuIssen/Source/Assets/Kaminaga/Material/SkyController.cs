using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SkyController : MonoBehaviour
{
    [Header("回転スピード")]
    public float m_rotateSpeed = 1.5f;

    [Header("現在の回転角度")]
    public float m_currentRot = 0.0f;

    // スカイボックスのマテリアル
    private Material m_skyBoxMat;

    private void Start()
    {
        // スカイボックスに設定されているマテリアルを取得
        m_skyBoxMat = RenderSettings.skybox;

        // スカイボックスが設定されているかどうかを判定
        if(m_skyBoxMat == null)
        {
            Debug.LogWarning("スカイボックスが見つかりません");
        }
        else
        {
            // スカイボックスの回転の初期値を設定
            m_skyBoxMat.SetFloat("_Rotation", m_currentRot);
        }
    }

    void FixedUpdate()
    {
        // スカイボックスが見つからない場合は処理をしない
        if(m_skyBoxMat == null) { return; }

        // Unity内で指定しているSkyboxの現在の回転を取得する
        m_currentRot = RenderSettings.skybox.GetFloat("_Rotation");

        // 次のフレームの回転を取得
        m_currentRot += (m_rotateSpeed * Time.deltaTime);

        // 最大角度(360度)を超えたら0に戻るようにする
        m_currentRot = Mathf.Repeat(m_currentRot, 360.0f);

        // 回転をスカイボックスにセットする
        m_skyBoxMat.SetFloat("_Rotation", m_currentRot);
    }

    private void OnDestroy()
    {
        // スカイボックスが見つからない場合は処理をしない
        if (m_skyBoxMat == null) { return; }

        // 回転をスカイボックスにセットする
        m_skyBoxMat.SetFloat("_Rotation", 0.0f);
    }
}
