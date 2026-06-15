using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq.Expressions;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.Android;
/// <summary>プレイヤーのanimNameのstring</summary>
namespace PlayerAnimName
{
    public static class AnimationNames
    {
    }

}







public partial class Player : MonoBehaviour
{
    [Header("<color=yellow>Player Settings</color>")]
    [SerializeField] float m_speed;
    [SerializeField] Animator m_anim;
    [SerializeField] PlayerState m_state = PlayerState.None;
    [SerializeField] PlayerState m_nextState = PlayerState.Idle;
    [SerializeField] UltimateState m_ultState = UltimateState.None;
    private const float m_attackInputStartTime = 0.06f;//攻撃入力が有効になる時間
    private const float m_attackInputEndTime = 0.9f;//攻撃入力が無効になる時間
    private const float m_attackAnimExecuteMinTime = 0.4f;//次のコンボに繋がるまでの攻撃アニメーションの最低時間
    private const float m_comboStartTime = 0.2f;//コンボ攻撃に派生する時間
    private readonly Vector3 m_jumpYAdd = new Vector3(0.0f, 6.0f, 0.0f);//ジャンプのときにy軸に加えるベクトル
    [SerializeField] int m_hp;//プレイヤーの体力
    [SerializeField] int Damage = 20;//プレイヤーが受けるダメージ量
    [SerializeField] float m_ultGage = 0;//アルティメットゲージ

    [Header("<color=green>Collider Objects</color>")]
    [SerializeField] Collider m_parryCol;//パリイの当たり判定
    [SerializeField] Collider m_attackCol;//攻撃の当たり判定
    [SerializeField] Collider m_hitCol;//プレイヤーの当たり判定
    [SerializeField] Collider m_parryAttackCol;//パリィ攻撃の当たり判定

    [Header("<color=cyan>Others</color>")]
    [SerializeField] Transform m_bossPos;//ボスの位置
    [SerializeField] BossHPController m_bossHpController;//ボスのHPコントローラー
    [SerializeField] BossAttack m_bossAttack;//ボスの攻撃コントローラー
    [SerializeField] Transform m_camera;//カメラのオブジェクト

    [Header("EffectManager")]
    [SerializeField] private EffectManager m_effectManager;

    [Header("SoundManager")]
    [SerializeField] private SoundManager m_soundManager;

    [Header("カメラ")]
    [SerializeField] private TPSCamera m_tpsCamera;

    [Header("カメラスイッチャー")]
    [SerializeField] private CameraSwitcher m_cameraSwitcher;
    [SerializeField] private float m_introCounter = 0;//イントロのカウンター//イントロの時間を計るためのもの
    bool m_isIntro = false;//イントロ中かどうか

    [Header("ShockWaveController")]
    [SerializeField] private ShockwaveController m_shockwaveController;

    bool OnYupTrigger;
    bool prevOnYupTrigger;
    //Getter
    /// <summary>
    /// playerのhpを取得
    /// </summary>
    /// <returns></returns>
    public int GetHP() { return m_hp; }
    /// <summary>
    /// プレイヤーのその時の攻撃力を取得する関数//現在のコンボノードの攻撃力を返す
    /// </summary>
    /// <returns></returns>
    public int GetAttackPower() { return m_comboNodeList.comboNodes[m_currentComboIndex].attackPower; }

    public float GetUltGage() { return m_ultGage; }
    public void IncreaseUltGage(int gage = 10) 
    {
        m_ultGage += gage;//アルティメットゲージを10増やす//仮の値
        if(m_ultGage > 100)
        {
            m_ultGage = 100;//最大値は100
        }
    }


    // Start is called before the first frame update
    void Start()
    {
        this.transform.position = new Vector3(0, -0.1f, 0);
        this.transform.rotation = Quaternion.Euler(0, 180, 0);
        m_angleH = 180;
        m_targetAngleH = 180;
        m_nextVel = this.transform.forward;
        ChangeState(PlayerState.Idle);
        //CSVからコンボデータを読み込む
        m_comboNodeList = CSVLoader.LoadComboNode("ComboChain");
        //当たり判定をオフにする
        m_parryCol.enabled = false;
        m_parryAttackCol.enabled = false;
        m_attackCol.enabled = false;
        //プレイヤーの当たり判定
        m_hitCol.enabled = true;
        m_cameraSwitcher.SwitchCamera(CameraSwitcher.CameraType.intro);
        m_isIntro = true;
    }


    void Update()
    {
        m_introCounter += Time.deltaTime;
        if (m_introCounter > FrameToSeconds(100) && m_isIntro)
        {
            m_cameraSwitcher.SwitchCamera(CameraSwitcher.CameraType.normal);
            m_isIntro = false;
        }

        if(m_cameraSwitcher.GetHighPriorityCamera() == CameraSwitcher.CameraType.intro)
        {
            return;
        }
            //初期化
            m_vel = Vector3.zero;
        //スティックの入力を受け取る
        horizontal = Input.GetAxis("Horizontal"); // 左右 (-1〜1)
        //horizontal *= -1;//反転
        vertical = Input.GetAxis("Vertical");   // 上下 (-1〜1)
        //vertical *= -1;//反転
        //デッドゾーンの設定
        if (Mathf.Abs(horizontal) < m_deadZone)
        {
            horizontal = 0;
        }
        if (Mathf.Abs(vertical) < m_deadZone)
        {
            vertical = 0;
        }   

        //buttonの入力の初期化//内部は連番になっているので、ループで回す
        for (int i = 0; i < buttonNum; i++)
        {
            m_buttons[i] = Input.GetKey((KeyCode)((int)KeyCode.JoystickButton0 + i));
            m_buttonsDown[i] = Input.GetKeyDown((KeyCode)((int)KeyCode.JoystickButton0 + i));
            m_buttonsUp[i] = Input.GetKeyUp((KeyCode)((int)KeyCode.JoystickButton0 + i));
        }

        //カメラからプレイヤーの向きのベクトルを求める
        MoveVec(ref m_forward, ref m_back, ref m_right, ref m_left);
        //PlayerのStateの中身の処理
        StatePattern();

        if (m_buttonsDown[(int)Button.RB] && m_ultGage >= 100)
        {
            //アルティメットの入力
            m_nextState = PlayerState.Ultimate;
            m_ultGage = 0;

            //プレイヤーのアルティメット時エフェクトを追加
            m_effectManager.PlayEffect("PlayerUltimateEffect", this.transform.position);

            //突っ込むときの音を入れる
            m_soundManager.PlaySE("StartUltimate");
        }
        //アルティメット攻撃のダメージ処理
        if (m_ultAttackDamaged)
        {
            //経過時間を加算
            m_ultAttackElapsedTime += Time.deltaTime * 10;
            //次のヒットタイミングをチェック
            if (m_ultAttackElapsedTime > m_ultAttackTimings[m_ultAttackhitIndex])
            {
                //ダメージを与える//ボスの被ダメ処理を呼ぶ
                m_bossHpController.OnDamage(15);//Ultは15;
                Debug.Log("<color=blue>Ultimate Attack Hit! Hit Index: </color>" + m_ultAttackhitIndex);
                m_ultAttackhitIndex++;
                //次のヒットタイミングがあるかをチェック
                if (m_ultAttackhitIndex >= m_ultAttackTimings.Length)
                {
                    //終わり
                    m_ultAttackDamaged = false;
                    m_ultAttackElapsedTime = 0;
                    m_ultAttackhitIndex = 0;
                    //IsTriggerを有効
                    m_hitCol.isTrigger = true;
                }


            }
        }
        //無敵時間の遷移
        if (m_isInvincible)
        {
            m_invincibleTimer -= Time.deltaTime;
            if(m_invincibleTimer <= 0)//被ダメしてから2秒間は無敵
            {
                m_isInvincible = false;
            }
        }


       // Debug.Log("<color=yellow>Current State: </color>" + m_state);
        //Debug.Log("<color=yellow>m_nextComboIndex == : </color>" + m_nextComboIndex);
        //Debug.Log("<color=yellow>m_currentComboIndex == : </color>" + m_currentComboIndex);
        //状態の遷移//当たり判定の遷移もここで行っている
        ChangeState(m_nextState);



        var tpsCamera = m_tpsCamera.GetComponent<TPSVirtualCamera>();
        bool cameraLock = tpsCamera.IsLockOn();
        //座標
        if (m_state != PlayerState.Dash)
        {
            m_vel = Vector3.Normalize(m_vel);
            if(cameraLock && m_state == PlayerState.Walk)
            {
                this.transform.position += m_vel * m_speed * Time.deltaTime * 0.5f;
            }
            else
            {
               this.transform.position += m_vel * m_speed * Time.deltaTime;
            }
        }
        else
        {
            //ダッシュ中は、ダッシュ中の速度で移動する
            this.transform.position += m_vel * Time.deltaTime;
        }
        float minX = -14.43f;
        float maxX = 14.63f;
        float minZ = -20.94f;
        float maxZ = 21.92f;
        float YupminX = -3.25f;
        float YupmaxX = 3.03f;
        float Yoffset = 0.2f;
        if(this.transform.position.x < minX)
        {
            this.transform.position = new Vector3(minX, this.transform.position.y, this.transform.position.z);
        }
        if(this.transform.position.x > maxX)
        {
            this.transform.position = new Vector3(maxX, this.transform.position.y, this.transform.position.z);
        }
        if(this.transform.position.z < minZ)
        {
            this.transform.position = new Vector3(this.transform.position.x, this.transform.position.y, minZ);
        }
        if(this.transform.position.z > maxZ)
        {
            this.transform.position = new Vector3(this.transform.position.x, this.transform.position.y, maxZ);
        }
        prevOnYupTrigger = OnYupTrigger;
        OnYupTrigger = (this.transform.position.x > YupminX && this.transform.position.x < YupmaxX);
        if (OnYupTrigger && !prevOnYupTrigger)
        {
            this.transform.position = new Vector3(this.transform.position.x, this.transform.position.y + Yoffset, this.transform.position.z);
        }
        else if(!OnYupTrigger && prevOnYupTrigger)
        {
            this.transform.position = new Vector3(this.transform.position.x, this.transform.position.y - Yoffset, this.transform.position.z);
        }

        if (cameraLock && m_state != PlayerState.Dash)
        {
            // ボスの位置がnullではない場合
            if (m_bossPos != null)
            {
                //常に敵の方向を向く
                Vector3 directionToBoss = m_bossPos.position - this.transform.position;
                m_targetAngleH = Mathf.Atan2(directionToBoss.x, directionToBoss.z) * Mathf.Rad2Deg;
            }
        }
        //回転
        m_angleH = Mathf.LerpAngle(m_angleH, m_targetAngleH, Time.deltaTime * 10);
        this.transform.rotation = Quaternion.Euler(0, m_angleH, 0);
        
    }


    private float FrameToSeconds(float frame, float fps = 60.0f)
    {
        return frame / fps;//フレーム数を秒数に変換する//60fpsの場合
    }
}
