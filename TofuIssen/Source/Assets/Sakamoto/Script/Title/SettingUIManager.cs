using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class SettingUIManager : MonoBehaviour
{
    [Header("各項目のボタン")]
    [SerializeField] private Button m_masterVolButton;
    [SerializeField] private Button m_bgmVolButton;
    [SerializeField] private Button m_seVolButton;
    [SerializeField] private Button m_voiceVolButton;

    [Header("各項目のスライダー")]
    [SerializeField] private Slider m_masterVolSlider;
    [SerializeField] private Slider m_bgmVolSlider;
    [SerializeField] private Slider m_seVolSlider;
    [SerializeField] private Slider m_voiceVolSlider;

    [Header("カメラ上下左右反転のトグル")]
    [SerializeField] private Toggle m_invertCameraVerticalToggle;
    [SerializeField] private Toggle m_invertCameraHorizontalToggle;

    [Header("閉じるボタン")]
    [SerializeField] private Button m_closeButton;

    //セッティングパネル
    [SerializeField] private GameObject m_settingPanel;

    //前フレームの縦入力
    private float m_prevVerticalInput = 0.0f;

    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        if (m_settingPanel.activeSelf)
        {
            //イベントシステム取得
            EventSystem eventSystem = EventSystem.current;

            //現在フォーカスが当たっているオブジェクトを取得する
            GameObject selectObj = eventSystem.currentSelectedGameObject;

            //axisは-1.0～1.0の値が入る
            float axis = Input.GetAxis("Horizontal");

            //デッドゾーンを手動設定
            if (Mathf.Abs(axis) < 0.2f)
            {
                axis = 0.0f;
            }

            //マスターボリュームの項目にフォーカスが当たってる場合
            if (selectObj == m_masterVolButton.gameObject)
            {
                //スライダーのvalueにaxisを足す
                m_masterVolSlider.value += axis * Time.deltaTime;
            }
            //BGMボリュームの項目にフォーカスが当たってる場合
            if (selectObj == m_bgmVolButton.gameObject)
            {
                //スライダーのvalueにaxisを足す
                m_bgmVolSlider.value += axis * Time.deltaTime;
            }
            //SEボリュームの項目にフォーカスが当たってる場合
            if (selectObj == m_seVolButton.gameObject)
            {
                //スライダーのvalueにaxisを足す
                m_seVolSlider.value += axis * Time.deltaTime;
            }
            //ボイスボリュームの項目にフォーカスが当たってる場合
            if (selectObj == m_voiceVolButton.gameObject)
            {
                //スライダーのvalueにaxisを足す
                m_voiceVolSlider.value += axis * Time.deltaTime;
            }

            //縦入力を取得
            float verticalInput = Input.GetAxis("Vertical");
            //スティックを上に倒した場合
            if (m_prevVerticalInput < 0.9f &&
                verticalInput > 0.9f)
            {
                //BGMボリュームの項目にフォーカスが当たっている場合
                if (selectObj == m_bgmVolButton.gameObject)
                {
                    //マスターボリュームの項目にフォーカスを当てる
                    eventSystem.SetSelectedGameObject(m_masterVolButton.gameObject);
                }
                //SEボリュームの項目にフォーカスが当たっている場合
                else if (selectObj == m_seVolButton.gameObject)
                {
                    //BGMボリュームの項目にフォーカスを当てる
                    eventSystem.SetSelectedGameObject(m_bgmVolButton.gameObject);
                }
                //ボイスボリュームの項目にフォーカスが当たっている場合
                else if (selectObj == m_voiceVolButton.gameObject)
                {
                    //SEボリュームの項目にフォーカスを当てる
                    eventSystem.SetSelectedGameObject(m_seVolButton.gameObject);
                }
                //カメラ上下反転の項目にフォーカスが当たっている場合
                else if (selectObj == m_invertCameraVerticalToggle.gameObject)
                {
                    //ボイスボリュームの項目にフォーカスを当てる
                    eventSystem.SetSelectedGameObject(m_voiceVolButton.gameObject);
                }
                //カメラ左右反転の項目にフォーカスが当たっている場合
                else if (selectObj == m_invertCameraHorizontalToggle.gameObject)
                {
                    //カメラ上下反転の項目にフォーカスを当てる
                    eventSystem.SetSelectedGameObject(m_invertCameraVerticalToggle.gameObject);
                }
                //閉じるの項目にフォーカスが当たっている場合
                else if (selectObj == m_closeButton.gameObject)
                {
                    //カメラ上下反転の項目にフォーカスを当てる
                    eventSystem.SetSelectedGameObject(m_invertCameraHorizontalToggle.gameObject);
                }
            }
            //下にスティックを倒した場合
            else if (m_prevVerticalInput > -0.7f &&
                verticalInput < -0.7f)
            {
                //マスターボリュームの項目にフォーカスが当たっている場合
                if (selectObj == m_masterVolButton.gameObject)
                {
                    //BGMボリュームの項目にフォーカスを当てる
                    eventSystem.SetSelectedGameObject(m_bgmVolButton.gameObject);
                }
                //BGMボリュームの項目にフォーカスが当たっている場合
                else if (selectObj == m_bgmVolButton.gameObject)
                {
                    //SEボリュームの項目にフォーカスを当てる
                    eventSystem.SetSelectedGameObject(m_seVolButton.gameObject);
                }
                //SEボリュームの項目にフォーカスが当たっている場合
                else if (selectObj == m_seVolButton.gameObject)
                {
                    //ボイスボリュームの項目にフォーカスを当てる
                    eventSystem.SetSelectedGameObject(m_voiceVolButton.gameObject);
                }
                //ボイスボリュームの項目にフォーカスが当たっている場合
                else if (selectObj == m_voiceVolButton.gameObject)
                {
                    //カメラ上下反転の項目にフォーカスを当てる
                    eventSystem.SetSelectedGameObject(m_invertCameraVerticalToggle.gameObject);
                }
                //カメラ上下反転の項目にフォーカスが当たっている場合
                else if (selectObj == m_invertCameraVerticalToggle.gameObject)
                {
                    //カメラ左右反転の項目にフォーカスを当てる
                    eventSystem.SetSelectedGameObject(m_invertCameraHorizontalToggle.gameObject);
                }
                //カメラ左右反転の項目にフォーカスが当たっている場合
                else if (selectObj == m_invertCameraHorizontalToggle.gameObject)
                {
                    //カメラ左右反転の項目にフォーカスを当てる
                    eventSystem.SetSelectedGameObject(m_closeButton.gameObject);
                }
            }
            //前フレームの縦入力を更新
            m_prevVerticalInput = verticalInput;
        }
    }

    public void OnOpenSetting()
    {
        //各ボタンのNavigationのモードをNoneにする
        Navigation navigation = new Navigation();
        navigation.mode = Navigation.Mode.None;
        //ボタンに適用する
        m_masterVolButton.navigation = navigation;
        m_bgmVolButton.navigation = navigation;
        m_seVolButton.navigation = navigation;
        m_voiceVolButton.navigation = navigation;
        //トグルに適用する
        m_invertCameraVerticalToggle.navigation = navigation;
        m_invertCameraHorizontalToggle.navigation = navigation;
    }

    public void OnCloseSetting()
    {
        //各ボタンのNavigationのモードをAutomaticにする
        Navigation navigation = new Navigation();
        navigation.mode = Navigation.Mode.Automatic;
        //ボタンに適用する
        m_masterVolButton.navigation = navigation;
        m_bgmVolButton.navigation = navigation;
        m_seVolButton.navigation = navigation;
        m_voiceVolButton.navigation = navigation;
        //トグルに適用する
        m_invertCameraVerticalToggle.navigation = navigation;
        m_invertCameraHorizontalToggle.navigation = navigation;
    }
}
