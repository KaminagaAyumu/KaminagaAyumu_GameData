using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// PlayerのState遷移とStateごとの挙動を管理する
/// </summary>
public partial class Player
{
    private readonly float m_comboChangeTime = 0.2f;//コンボ攻撃に派生する時間

    /// <summary>
    /// 次の状態に遷移する(遷移しなかったら、早期リターン)
    /// </summary>
    /// <param name="nextState"></param>
    void ChangeState(PlayerState nextState)
    {
        //攻撃のコンボ遷移の処理
        if (m_state == PlayerState.Attack && nextState == PlayerState.Attack && m_nextComboIndex != 0 && m_isComboExecuting)
        {
            //アニメーション割合以上じゃなかったら早期リターンする
            AnimatorStateInfo stateInfo = m_anim.GetCurrentAnimatorStateInfo(0);
            if (stateInfo.normalizedTime < m_comboChangeTime)
            {
                return;
            }

            m_currentComboIndex = m_nextComboIndex;
            //当たり判定をオフにする
            m_attackCol.enabled = false;
            //体の向きを変更
            CantMoveInfo(m_forward, m_back, m_right, m_left);
            m_targetAngleH = Mathf.Atan2(m_nextVel.x, m_nextVel.z) * Mathf.Rad2Deg;
            //コンボ攻撃をする
            switch (m_nextComboIndex)
            {
                case 0:
                    m_anim.SetTrigger("isAttack");
                    break;
                case 1:
                    m_anim.SetTrigger("isAttack2");
                    Debug.Log("isAttack2 がONになりました");
                    break;
                case 2:
                    m_anim.SetTrigger("isAttack3");
                    break;
                case 3:
                    m_anim.SetTrigger("isAttack4");
                    break;
                case 4:
                    m_anim.SetTrigger("isAttack5");
                    break;
                default:
                    break;
            }
            return;
        }

        if (m_state == nextState) return;
        //現在の状態を終了する----------------------------------------------
        switch (m_state)
        {
            case PlayerState.Idle:
                //次のStateがDashの時、プレイヤーの現在の向きを渡す
                if (nextState == PlayerState.Dash) m_dashVec = this.transform.forward;
                break;
            case PlayerState.Walk:
                if (nextState == PlayerState.Dash) m_dashVec = m_vel;
                m_soundManager.StopSE("PlayerRun");
                m_soundManager.StopSE("RockOnWalk");
                //カメラのロックオンに合わせて変える
                var tpsCamera = m_tpsCamera.GetComponent<TPSVirtualCamera>();
                bool cameraLock = tpsCamera.IsLockOn();
                if (cameraLock)
                {
                    m_anim.SetBool("isWalkStrafe", false);
                    Debug.Log("isStrafe がOFFになりました");
                }
                else
                {
                    m_anim.SetBool("isWalk", false);
                }
                break;
            case PlayerState.Run:
                break;
            case PlayerState.Attack:
                // Attack状態を抜ける時にリセット
                m_currentComboIndex = 0;
                m_nextComboIndex = 0;
                m_nextComboIndex = 0;//攻撃アニメーションのインデックスをリセットする
                m_nextComboIndex = 0;
                m_isComboReserved = false;
                m_isComboExecuting = false;
                m_attackCol.enabled = false;
                //攻撃アニメーションのトリガーをリセットする
                m_anim.ResetTrigger("isAttack");
                m_anim.ResetTrigger("isAttack2");
                m_anim.ResetTrigger("isAttack3");
                m_anim.ResetTrigger("isAttack4");
                m_anim.ResetTrigger("isAttack5");
                //移行先がDashの時、m_nextVecを渡す
                if (nextState == PlayerState.Dash)
                {
                    if (m_nextVel != Vector3.zero)
                    {
                        m_dashVec = m_nextVel;
                    }
                    else
                    {
                        m_dashVec = this.transform.forward;
                    }
                }
                //m_nextVelをリセットする
                m_nextVel = Vector3.zero;
                break;
            case PlayerState.Dash:
                //m_dashVecをリセットする
                m_dashVec = Vector3.zero;
                m_dashTime = 0;
                m_anim.SetBool("isDash", false);
                //無敵をやめる
                m_hitCol.enabled = true;
                break;
            case PlayerState.Parry:
                if (nextState != PlayerState.ParryCounter) m_anim.SetTrigger("isParryEnd");
                m_anim.SetBool("isParry", false);
                m_parryCol.enabled = false;
                m_hitCol.enabled = true;//パリイをやめたら、プレイヤーの当たり判定を有効にする
                //移行先がDashの時、m_nextVecを渡す
                if (nextState == PlayerState.Dash)
                {
                    if (m_nextVel != Vector3.zero)
                    {
                        m_dashVec = m_nextVel;
                    }
                    else
                    {
                        m_dashVec = this.transform.forward;
                    }
                }
                //m_nextVelをリセットする
                m_nextVel = Vector3.zero;
                break;
            case PlayerState.ParryCounter:
                if (!m_isParryCounterAttack) m_anim.SetTrigger("isParryEnd");//攻撃をしていない場合は、通常のパリイの終了アニメーションを再生する
                m_isParryCounterAttack = false;
                m_anim.ResetTrigger("isParryCounter");
                //当たり判定を有効にする
                m_hitCol.enabled = true;
                //攻撃の当たり判定を無効にする
                m_parryAttackCol.enabled = false;
                //通常カメラに戻す

                if(m_cameraSwitcher.GetHighPriorityCamera() != CameraSwitcher.CameraType.end)
                {
                    m_cameraSwitcher.SwitchCamera(CameraSwitcher.CameraType.normal);
                }
                break;
            case PlayerState.Ultimate:
                m_ultState = UltimateState.None;
                //当たり判定を有効
                m_hitCol.enabled = true;
                m_anim.ResetTrigger("isUltAttack");
                break;
            case PlayerState.Damage:
                m_anim.SetBool("isHit", false);
                break;
            case PlayerState.Death:
                //当たり判定を消す
                m_hitCol.enabled = false;
                break;
            default:
                break;
        }
        m_state = nextState;
        //次の状態を開始する---------------------------------------------
        switch (m_state)
        {
            case PlayerState.Idle:
                break;
            case PlayerState.Walk:
                //カメラのロックオンに合わせて変える
                var tpsCamera = m_tpsCamera.GetComponent<TPSVirtualCamera>();
                bool cameraLock = tpsCamera.IsLockOn();
                if ((cameraLock))
                {
                    m_anim.SetBool("isWalkStrafe", true);
                    Debug.Log("isStrafe がONになりました");
                }
                else
                {
                    m_anim.SetBool("isWalk", true);
                }
                break;
            case PlayerState.Run:
                break;
            case PlayerState.Attack:

                if (m_nextComboIndex == 0)
                {
                    m_currentComboIndex = m_nextComboIndex;
                    m_anim.SetTrigger("isAttack");
                }

                ////素振り音を再生
                //m_soundManager.PlaySE("PlayerAirSlash");

                break;
            case PlayerState.Dash:
                m_anim.SetBool("isDash", true);
                //無敵にする
                m_hitCol.enabled = false;
                //エフェクトの再生
                m_effectManager.PlayEffect("AvoidancePlayer", this.transform.position);
                break;
            case PlayerState.Parry:
                m_anim.SetBool("isParry", true);
                m_anim.ResetTrigger("isParryEnd");
                m_parryCol.enabled = true;
                m_justParryFrame = 0;
                //当たり判定を無効
                m_hitCol.enabled = false;
                break;
            case PlayerState.ParryCounter:
                m_anim.ResetTrigger("isParryCounter");
                m_parryCounterFrame = 0;
                m_isparryCounterReserved = false;
                m_isAttackOnce = false;
                //当たり判定を無効
                m_hitCol.enabled = false;
                break;
            case PlayerState.Ultimate:
                m_ultState = UltimateState.UltMove;
                //角度をボスの方向に向ける
                //Playerをcolliderの方向に向ける//ワールド座標での角度が帰ってくる
                m_targetAngleH = Mathf.Atan2(m_bossPos.transform.position.x - this.transform.position.x, m_bossPos.transform.position.z - this.transform.position.z) * Mathf.Rad2Deg;
                //当たり判定を無効
                m_hitCol.enabled = false;
                //IsTriggerも無効
                m_hitCol.isTrigger = false;
                m_anim.SetTrigger("isUltMove");
                break;
            case PlayerState.Damage:
                m_anim.SetBool("isHit", true);
                m_isInvincible = true;//無敵状態にする
                m_invincibleTimer = FrameToSeconds(120);
                m_damageTiemr = 0;
                break;
            case PlayerState.Death:
                m_anim.SetBool("isDead", true);
                m_isInvincible = true;
                break;
            default:
                break;
        }

    }
    /// <summary>
    /// PlayerのState遷移
    /// </summary>
    void StatePattern()
    {
        switch (m_state)
        {
            case PlayerState.Idle:
                //入力に応じて、速度を加える
                InputMove(m_forward, m_back, m_right, m_left);
                //プレイヤーの移動方向に応じて、プレイヤーの向きを変える
                if (m_vel != Vector3.zero)
                {
                    m_targetAngleH = Mathf.Atan2(m_vel.x, m_vel.z) * Mathf.Rad2Deg;//2πを360度に変換するために、Rad2Degを掛ける//Mathf.Rad2Degは180/πの値を持つ定数
                }
                //spaceを押したら攻撃
                if (m_buttonsDown[(int)Button.X])
                {
                    m_nextState = PlayerState.Attack;
                }
                //パリイの入力
                if (m_buttonsDown[(int)Button.LB])
                {
                    m_nextState = PlayerState.Parry;
                }
                //ダッシュの入力
                if (m_buttonsDown[(int)Button.A])
                {
                    m_nextState = PlayerState.Dash;

                    //エフェクトの再生
                    m_effectManager.PlayEffect("AvoidancePlayer", this.transform.position);

                    //音を再生
                    m_soundManager.PlaySE("AvoidancePlayer");
                }
                break;
            case PlayerState.Walk:
                //入力に応じて、速度を加える
                InputMove(m_forward, m_back, m_right, m_left);
            


                //プレイヤーの移動方向に応じて、プレイヤーの向きを変える
                if (m_vel != Vector3.zero)
                {
                    m_targetAngleH = Mathf.Atan2(m_vel.x, m_vel.z) * Mathf.Rad2Deg;//2πを360度に変換するために、Rad2Degを掛ける//Mathf.Rad2Degは180/πの値を持つ定数
                }
                //spaceを押したら攻撃
                if (m_buttonsDown[(int)Button.X])
                {
                    m_nextState = PlayerState.Attack;
                }
                //パリイの入力
                if (m_buttonsDown[(int)Button.LB])
                {
                    m_nextState = PlayerState.Parry;
                }
                //ダッシュの入力
                if (m_buttonsDown[(int)Button.A])
                {
                    m_nextState = PlayerState.Dash;

                    //エフェクトの再生
                    m_effectManager.PlayEffect("AvoidancePlayer", this.transform.position);
                    //音を再生
                    m_soundManager.PlaySE("AvoidancePlayer");
                }
                //カメラのロックオンが変わったら変える//もしかしたらバグるかも
                var tpsCamera = m_tpsCamera.GetComponent<TPSVirtualCamera>();
                bool cameraLock = tpsCamera.IsLockOn();
                if (cameraLock)
                {
                    m_anim.SetBool("isWalkStrafe", true);
                    m_anim.SetBool("isWalk", false);
                    //アニメーション
                    if (horizontal >= 0)//右
                    {
                        m_anim.SetFloat("MoveX", 1);
                    }
                    else//左
                    {
                        m_anim.SetFloat("MoveX", -1);
                    }
                }
                else
                {
                    m_anim.SetBool("isWalkStrafe", false);
                    m_anim.SetBool("isWalk", true);
                }

                break;
            case PlayerState.Run:
                break;
            case PlayerState.Attack:
                AttackInfo();//攻撃の挙動を管理する関数
                CantMoveInfo(m_forward, m_back, m_right, m_left);//攻撃中の移動入力を受け取る
                //パリイの入力
                if (m_buttonsDown[(int)Button.LB])
                {
                    m_nextState = PlayerState.Parry;
                }
                //ダッシュの入力
                if (m_buttonsDown[(int)Button.A])
                {
                    m_nextState = PlayerState.Dash;
                }
                break;
            case PlayerState.Dash:
                m_vel = Vector3.Normalize(m_dashVec) * m_speed * 2;//ダッシュの速度は通常の速度の2倍とする
                //ダッシュの時間
                if (m_dashTime < FrameToSeconds(20))//ダッシュの時間フレーム
                {
                    m_dashTime += Time.deltaTime;
                }
                else
                {
                    m_dashTime = 0;
                    m_nextState = PlayerState.Idle;
                }
                break;
            case PlayerState.Parry:
                //パリイの挙動
                //Rを離したらIdleに遷移する
                m_justParryFrame += Time.deltaTime;
                CantMoveInfo(m_forward, m_back, m_right, m_left);//パリイ中の移動入力を受け取る
                if (m_justParryFrame > FrameToSeconds(60))//ジャストパリイになるフレーム数
                {
                    m_hitCol.enabled = true;//ジャストパリイの時間を過ぎたら、プレイヤーの当たり判定を有効にする
                }

                //パリイボタンを離したら、Idleに遷移する
                if (m_buttonsUp[(int)Button.LB])
                {
                    m_nextState = PlayerState.Idle;
                }
                //ダッシュの入力
                if (m_buttonsDown[(int)Button.A])
                {
                    m_nextState = PlayerState.Dash;
                    //エフェクトの再生
                    m_effectManager.PlayEffect("AvoidancePlayer", this.transform.position);
                    //音を再生
                    m_soundManager.PlaySE("AvoidancePlayer");
                }
                break;
            case PlayerState.ParryCounter:
                //パリイカウンターの挙動
                //ボタンを押したら確定反撃に移る
                //何フレームか経って、ボタンが押されなかったらIdleに移る
                m_parryCounterFrame += Time.deltaTime;

                ///すぐにParryCounterすると、バグる原因になるから、予約システムにすると、いいかもしれない//また、ここでもParryを押せるようにする

                if (m_buttonsDown[(int)Button.X] && !m_isparryCounterReserved && !m_isAttackOnce)
                {
                    m_isparryCounterReserved = true;
                    m_isAttackOnce = true;
                }
                //予約システムにする
                if (m_parryCounterFrame > FrameToSeconds(30) && m_isparryCounterReserved && !m_isParryCounterAttack)//
                {
                    //確定反撃の処理
                    Debug.Log("<color=magenta>Parry Counter!</color>");
                    m_isParryCounterAttack = true;
                    //確定反撃のアニメーションを再生する
                    m_anim.SetTrigger("isParryCounter");
                    m_parryAttackCol.enabled = true;
                    //パリィカウンターの音を鳴らす
                    m_soundManager.PlaySE("ParryCounter");
                }
                

                if (m_isParryCounterAttack)
                {
                    AnimatorStateInfo stateInfo = m_anim.GetCurrentAnimatorStateInfo(0);
                    if (stateInfo.IsName("ParryCounterAttack") && stateInfo.normalizedTime >= 1.0f)
                    {
                        m_nextState = PlayerState.Idle;
                    }
                    // ★ 安全タイムアウト：アニメーション名不一致などの場合でも脱出できるようにする
                    else if (m_parryCounterFrame >= FrameToSeconds(60))
                    {
                        Debug.Log("<color=red>ParryCounter timeout: アニメーションが終了しなかったため強制的にIdleへ移行</color>");
                        m_nextState = PlayerState.Idle;
                    }
                }
                if (m_parryCounterFrame >= FrameToSeconds(30) && !m_isParryCounterAttack)
                {
                    m_nextState = PlayerState.Idle;
                }

                break;
            case PlayerState.Ultimate:
                switch (m_ultState)
                {
                    case UltimateState.None:
                        break;
                    case UltimateState.UltMove:
                        //ボスのposを検知し、その方向に向かって、移動→たどり着いたら、高く飛び上がって、攻撃をする、みたいな感じにする予定
                        //m_bossPosとPlayerのベクトルを作る
                        Vector3 bossDir = m_bossPos.position - this.transform.position;
                        Vector3 bossOffset = -bossDir.normalized *5.0f;//ボスから少し離れた位置を目標地点にするためのオフセット
                        //LerpでPlayerの座標ををbossDirの方向に向ける
                       // Vector3 offset = new Vector3(0, 1, 0);//
                        Vector3 targetPos = m_bossPos.position;
                        this.transform.position = Vector3.Lerp(this.transform.position, targetPos, Time.deltaTime *4.0f);
                        //別で上ベクトルにも移動
                        //this.transform.position += offset * Time.deltaTime * 7f;
                        Vector3 dis = this.transform.position - targetPos;
                        dis.y = 0;


                        //if (dis.magnitude < 1.0f)//目標地点に近づいたら、UltJumpに移行する
                        //{
                        //    m_ultState = UltimateState.UltJump;
                        //    m_ultJumpTargetPos = this.transform.position + m_jumpYAdd;//ジャンプの目標地点を設定する
                        //    m_anim.ResetTrigger("isUltMove");
                        //    m_anim.SetTrigger("isUltAttack");
                        //}

                        break;
                    case UltimateState.UltJump:
                        //ここでは高く飛び上がる
                        this.transform.position = Vector3.Lerp(this.transform.position, m_ultJumpTargetPos, Time.deltaTime * 10);



                        if (Vector3.Distance(this.transform.position, m_ultJumpTargetPos) < 0.01f)
                        {
                            m_ultState = UltimateState.UltAttack;
                        }

                        break;
                    case UltimateState.UltAttack:
                        //ここでは下に行く
                        this.transform.position = Vector3.Lerp(this.transform.position, new Vector3(this.transform.position.x, -2.0f, this.transform.position.z), Time.deltaTime * 10);

                        if (this.transform.position.y <= 1.0f)//地面に着いたら、アルティメット攻撃のエフェクトを出すなどの処理をする
                        {
                            this.transform.position = new Vector3(this.transform.position.x, 0.0f, this.transform.position.z);
                            //アルティメット攻撃のエフェクトを出すなどの処理をする
                            m_nextState = PlayerState.Idle;
                            m_ultState = UltimateState.None;
                            //ダメージを出す
                            m_ultAttackDamaged = true;

                            //エフェクトを出す
                            m_effectManager.PlayEffect("PlayerLandingEffect", this.transform.position);
                            //音を鳴らす
                            m_soundManager.PlaySE("EndUltimate");

                            //カメラを揺らす
                            m_tpsCamera.StartShakeCamera(0.5f, 0.5f);
                        }
                        break;
                    default:
                        break;
                }
                break;
            case PlayerState.Damage:
                m_damageTiemr += Time.deltaTime;

                if (m_damageTiemr > FrameToSeconds(60))//被ダメして1秒動けない
                {
                    m_nextState = PlayerState.Idle;
                }
                break;
            case PlayerState.Death:
                break;
            default:
                break;
        }
    }
}
