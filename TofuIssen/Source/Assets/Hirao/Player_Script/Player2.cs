using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// Playerの変数や、enumをまとめるクラス
/// </summary>
public partial class Player
{



    //CSVから読み込んだコンボデータ
    private ComboNodeList m_comboNodeList;
    private ComboNode m_currentComboNode;

    private Vector3 m_vel;
    private Vector3 m_nextVel;//攻撃時などの動けないときに取る入力
    private Vector3 m_dashVec;//ダッシュの入力を保存する変数
    private Vector3 m_ultJumpTargetPos;//アルティメットのジャンプの目標地点
    private Vector3 m_forward;
    private Vector3 m_back;
    private Vector3 m_right;
    private Vector3 m_left;
    private float m_angleH;
    private float m_targetAngleH;
    private bool m_isComboReserved = false;//コンボ攻撃が予約されているかどうかを示すフラグ
    private bool m_isComboExecuting = false;//コンボ攻撃が実行されているかどうかを示すフラグ
    private bool m_isParryCounterAttack = false;//パリイカウンター攻撃をしているかどうか
    private int m_currentComboIndex = 0;//現在の攻撃インデックス
    private int m_nextComboIndex = 0;//次の攻撃アニメーションのインデックスを管理する変数
    private float m_justParryFrame = 0;//ジャストパリイになるカウント
    private float m_parryCounterFrame = 0;//パリイカウンターになるカウント
    bool m_isparryCounterReserved = false;//パリイカウンターが予約されているかどうかを示すフラグ
    /// <summary>スティックの左右入力値(-1〜1)</summary>
    private float horizontal;
    /// <summary>スティックの左右入力値(-1〜1)</summary>
    private float vertical;
    //パッドのデッドゾーン
    private const float m_deadZone = 0.2f; // デッドゾーンの閾値
    //ダッシュの時間を管理する変数
    private float m_dashTime = 0;
    //Ult用の変数
    private bool m_ultAttackDamaged = false;//アルティメット攻撃がダメージを与える旗
    private float m_ultAttackElapsedTime = 0;//経過時間
    private int m_ultAttackhitIndex = 0;//現在のヒット番号
    private readonly float[] m_ultAttackTimings = new float[] { 0.8f, 2.0f, 3.2f, 4.4f, 5.4f, 6.2f };//攻撃が当たるタイミング//秒
    //被ダメ関連
    private float m_damageTiemr = 0;//被ダメしてからの動けない時間
    private float m_invincibleTimer = 0;//無敵時間
    private bool m_isInvincible = false;//無敵状態かどうか
    //UltのLerp用の変数
    private float m_ultMoveTime = 0.0f;
    //parryAttack用のbool
    private bool m_isAttackOnce = false;//攻撃を一度でも行ったかどうか

    //ボタン
    /// <summary>
    /// A, B, X, Y, LB, RB, LT, RT, Back, Startの順
    /// </summary>
    private const int buttonNum = 10;
    /// <summary>
    /// A=0, B=1, X=2, Y=3, LB=4, RB=5, LT=6, RT=7, Back=8, Start=9の順
    /// </summary>
    private bool[] m_buttons = new bool[buttonNum];
    private bool[] m_buttonsDown = new bool[buttonNum];
    private bool[] m_buttonsUp = new bool[buttonNum];
    public enum Button : int
    {
        A = 0,
        B = 1,
        X = 2,
        Y = 3,
        LB = 4,
        RB = 5,
        LT = 6,
        RT = 7,
        Back = 8,
        Start = 9,
    }

    enum PlayerState : int
    {
        None = 0,
        Idle,
        Walk,
        Run,
        Attack,
        Dash,
        Parry,
        ParryCounter,
        Ultimate,
        Damage,
        Death,
    }
    enum UltimateState : int
    {
        None = 0,
        UltMove,
        UltJump,
        UltAttack,
    }

  
}
