using System.Collections;
using System.Collections.Generic;
using UnityEngine;


/// <summary>
/// Playerの関数をまとめる
/// </summary>
public partial class Player
{



   
    /// <summary>
    /// カメラから見たプレイヤーの前後左右のベクトルを求める関数
    /// </summary>
    /// <param name="forward"></param>
    /// <param name="down"></param>
    /// <param name="right"></param>
    /// <param name="left"></param>
    void MoveVec(ref Vector3 forward, ref Vector3 down, ref Vector3 right, ref Vector3 left)
    {
        //カメラから見て、前後左右の入力を受け取る
        //Vector3 camerapos = Camera.main.transform.position;
        Vector3 camerapos = m_camera.transform.position;
        //カメラからプレイヤーの向きのベクトルを求める
        forward = this.transform.position - camerapos;
        down = -forward;
        right = Vector3.Cross(Vector3.up, forward);
        left = -right;
        //y成分を0にして、正規化する
        forward.y = 0;
        down.y = 0;
        right.y = 0;
        left.y = 0;
        forward.Normalize();
        down.Normalize();
        right.Normalize();
        left.Normalize();
    }
    /// <summary>
    /// 入力をしたら、速度を加える
    /// </summary>
    /// <param name="forward"></param>
    /// <param name="down"></param>
    /// <param name="right"></param>
    /// <param name="left"></param>
    void InputMove(Vector3 forward, Vector3 down, Vector3 right, Vector3 left)
    {
#if UNITY_EDITOR
        if (Input.GetKey(KeyCode.UpArrow) || vertical > 0)
        {
            m_vel += forward;
        }

        if (Input.GetKey(KeyCode.DownArrow) || vertical < 0)
        {
            m_vel += down;
        }

        if (Input.GetKey(KeyCode.RightArrow) || horizontal > 0)
        {
            m_vel += right;
        }

        if (Input.GetKey(KeyCode.LeftArrow) || horizontal < 0)
        {
            m_vel += left;
        }
#else
        if ( vertical > 0)
        {
            m_vel += forward;
        }

       if ( vertical < 0)
        {
            m_vel += down;
        }

       if ( horizontal > 0)
        {
            m_vel += right;
        }

       if ( horizontal < 0)
        {
            m_vel += left;
        }
#endif
        //速度によって、次の状態を決める
        if (m_vel == Vector3.zero)
        {
            m_nextState = PlayerState.Idle;
            //歩き音を止める
            m_soundManager.StopSE("PlayerRun");
        }
        else
        {
            m_nextState = PlayerState.Walk;
            //回避中ではないなら
            if(m_nextState != PlayerState.Dash)
            {
                var tpsCamera = m_tpsCamera.GetComponent<TPSVirtualCamera>();
                bool cameraLock = tpsCamera.IsLockOn();
                //ロックオン中なら
                if (cameraLock)
                {
                    //ロックオン中の歩き音を鳴らす
                    m_soundManager.PlaySE("RockOnWalk");
                }
                else
                {
                    //通常走り音を鳴らす
                    m_soundManager.PlaySE("PlayerRun");
                }
                

            }
            //回避中なら音を止める
            else
            {
                m_soundManager.StopSE("PlayerRun");
            }
        }
    }
    /// <summary>
    /// StateがAttackのときの処理をする関数
    /// </summary>
    void AttackInfo()
    {
        //攻撃の挙動
        //現在のアニメーション状態を取得
        AnimatorStateInfo stateInfo = m_anim.GetCurrentAnimatorStateInfo(0);
        if (stateInfo.IsName(m_comboNodeList.comboNodes[0].animName) ||
            stateInfo.IsName(m_comboNodeList.comboNodes[1].animName) ||
            stateInfo.IsName(m_comboNodeList.comboNodes[2].animName) ||
            stateInfo.IsName(m_comboNodeList.comboNodes[3].animName) ||
            stateInfo.IsName(m_comboNodeList.comboNodes[4].animName)
            )
        {
            //正規化された時間(0.0～1.0)
            float normalizedTime = stateInfo.normalizedTime;
            //コンボ予約
            if (normalizedTime >= m_attackInputStartTime && normalizedTime < m_attackInputEndTime)
            {
                //ここでボタンを押されたら、コンボ攻撃に派生するシステムを
                if (m_buttonsDown[(int)Button.X] && !m_isComboReserved)
                {
                    //現在のコンボを把握して、予約するかを判断する

                    //コンボ攻撃に派生するシステムを作る
                    m_isComboReserved = true;
                }

            }
            //当たり判定
            if (normalizedTime >= 0.1f && normalizedTime < 0.6f)
            {
                m_attackCol.enabled = true;
            }
            else
            {
                m_attackCol.enabled = false;
            }


            //コンボ実行中ではないフラグ//Unityはアニメーションの遷移がよくわからないため
            if (normalizedTime < m_comboStartTime)
            {
                //アニメーションの割合が0.5未満に達したら、コンボ実行中のフラグをリセットする//現在のノードに合わせて変えれば良し
                m_isComboExecuting = false;
            }

            //現在のコンボ攻撃のノードを取得する
            ComboNode comboNode = m_comboNodeList.comboNodes[m_currentComboIndex];

            // 攻撃アニメーションが一定フレーム経過した後に次のコンボが予約されていたら次のコンボに移行する処理
            if (normalizedTime >= m_attackAnimExecuteMinTime && m_isComboReserved && !m_isComboExecuting)//ここを技ごとに本来はかえる
            {
                //現在のコンボ攻撃のノードを取得する
                //ComboNode comboNode = m_comboNodeList.comboNodes[m_currentComboIndex];
                //次の攻撃アニメーションのインデックスを取得する//空でなければ
                if (comboNode.nextLightAttack.Length > 0)
                {
                    m_nextComboIndex = comboNode.nextLightAttack[0];//次の攻撃アニメーションのインデックスを取得する
                                                                    //次の攻撃アニメーションに遷移する
                    m_isComboReserved = false;
                    m_isComboExecuting = true;
                    m_nextState = PlayerState.Attack;
                    Debug.Log("<color=red>Combo予約完了 m_nextComboIndex == </color>" + m_nextComboIndex);
                }
            }
            //移動距離
          
            if(normalizedTime <= comboNode.moveFrame)
            {
                m_vel = this.transform.forward * comboNode.moveSpeed;
            }
            else
            {
                m_vel = Vector3.zero;
            }

            //攻撃アニメーションが終了したら、Idle状態に遷移する
            if (normalizedTime >= 0.9f)
            {
                m_currentComboIndex = 0;
                m_nextComboIndex = 0;
                m_isComboExecuting = false;
                m_nextState = PlayerState.Idle;
            }


        }
    }
    /// <summary>
    /// 移動できない時の入力を保存する関数
    /// </summary>
    /// <param name="forward"></param>
    /// <param name="back"></param>
    /// <param name="right"></param>
    /// <param name="left"></param>
    void CantMoveInfo(Vector3 forward, Vector3 back, Vector3 right, Vector3 left)
    {
        //今回のフレームで入力があったか
        Vector3 inputNowFrame = Vector3.zero;

        if (vertical > 0)
        {
            inputNowFrame += forward;
        }

        if (vertical < 0)
        {
            inputNowFrame += back;
        }

        if (horizontal > 0)
        {
            inputNowFrame += right;
        }

        if (horizontal < 0)
        {
            inputNowFrame += left;
        }
        //今回のフレームで入力があったら、それを保存する
        if (inputNowFrame != Vector3.zero) m_nextVel = inputNowFrame;
        if(inputNowFrame == Vector3.zero && m_nextVel == Vector3.zero)
        {
            m_nextVel = this.transform.forward;//入力がないときは、前を向くようにする
        }

    }
    /// <summary>
    /// パリイの当たり判定に入ったときの処理
    /// </summary>
    /// <param name="other"></param>
    public void ParryHit(Collider other)
    {
        //ジャストパリイの処理
        if (m_justParryFrame <= FrameToSeconds(18))//ジャストパリイになるフレーム数
        {
            Debug.Log("<color=cyan>Just Parry!</color>");
            //ジャストパリイのエフェクトを出すなどの処理
            //ジャストパリィ音を鳴らす
            m_soundManager.PlaySE("JustParry");
            m_shockwaveController.OnShockwave();

            //Playerをcolliderの方向に向ける//ワールド座標での角度が帰ってくる
            m_targetAngleH = Mathf.Atan2(m_bossPos.transform.position.x - this.transform.position.x, m_bossPos.transform.position.z - this.transform.position.z) * Mathf.Rad2Deg;

            //パリィカメラに切り替える
            m_cameraSwitcher.SwitchCamera(CameraSwitcher.CameraType.parry);//カメラを切り替える

            //Stateを移行させる
            m_nextState = PlayerState.ParryCounter;
            ChangeState(m_nextState);//状態を移行させる
        }
        else
        {
            //Playerをcolliderの方向に向ける//ワールド座標での角度が帰ってくる
            m_targetAngleH = Mathf.Atan2(m_bossPos.transform.position.x - this.transform.position.x, m_bossPos.transform.position.z - this.transform.position.z) * Mathf.Rad2Deg;
            Debug.Log("<color=cyan>Parry!</color>");
            //通常のパリイのエフェクトを出すなどの処理
            m_soundManager.PlaySE("Parry");
        }
    }
    /// <summary>
    /// プレイヤーの被ダメの処理
    /// </summary>
    /// <param name="other"></param>
    public void PlayerHit(Collider other)
    {
        //敵の攻撃の当たり判定に入ったときの処理
        if (other.CompareTag("EnemyAttack"))
        {
            if (m_invincibleTimer > 0)//無敵時間中なら
            {
                return;
            }
            if (m_state == PlayerState.Parry)//パリイ中なら
            {
                //いったんこれ
                return;
            }
            m_hp -= m_bossAttack.GetBossAttackDamage();

            //HPをクランプする
            m_hp = Mathf.Clamp(m_hp, 0, 100);
            if (m_hp > 0)
            {
                //被ダメする
                m_nextState = PlayerState.Damage;
               // m_hp -= m_bossAttack.SetAttackType();
                //Playerをcolliderの方向に向ける//ワールド座標での角度が帰ってくる
                m_targetAngleH = Mathf.Atan2(m_bossPos.transform.position.x - this.transform.position.x, m_bossPos.transform.position.z - this.transform.position.z) * Mathf.Rad2Deg;

                //ダメージエフェクトを再生する
                //エフェクトを出す位置を計算
                Vector3 effectPos = this.transform.position;
                effectPos.y += 1.0f;//this.transform.positionは足元なので少し上にする

                m_effectManager.PlayEffect("PlayerDamageEffect", effectPos);
            }
            else
            {
                //死亡する
                m_nextState = PlayerState.Death;
                
            }
            ChangeState(m_nextState);


        }
    }
    /// <summary>
    /// UltMoveからUltJumpに移行する関数
    /// </summary>
    /// <param name="other"></param>
    public void UltimateHit(Collider other)
    {
        if(other.CompareTag("Enemy"))
        {
            Debug.Log("<color=magenta>UltimateHit!</color>");
            if (m_state != PlayerState.Ultimate) return;
            if(m_ultState == UltimateState.UltMove)
            {
                m_ultState = UltimateState.UltJump;
                m_ultJumpTargetPos = this.transform.position + m_jumpYAdd;//ジャンプの目標地点を設定する
                m_anim.ResetTrigger("isUltMove");
                m_anim.SetTrigger("isUltAttack");
            }
        }
        
    }

    /// <summary>
    /// プレイヤーの押し戻し
    /// </summary>
    /// <param name="other"></param>
    public void PlayerPushBack(Collider other)
    {
        if (other.CompareTag("Enemy"))
        {
            //プレイヤとボスの押し戻し判定
            Vector3 playerPos = m_hitCol.bounds.center;
            CapsuleCollider capsule = m_hitCol as CapsuleCollider;
            float playerRadius = 0;
            if (capsule != null)
            {
                playerRadius = capsule.radius;
            }
            Vector3 bossPos = other.bounds.center;
            float bossRadius = other.bounds.extents.x;

            Vector3 pushDir = bossPos - playerPos;
            pushDir.y = 0;

            float dist = pushDir.magnitude;
            float overlap = playerRadius + bossRadius - dist;
         //  Debug.Log("<color=magenta>PlayerPushBack! overlap == </color>" + overlap);
            if (overlap > 0)
            {
                pushDir.Normalize();
                Vector3 pushBack = pushDir * overlap;
                this.transform.position -= pushBack;
           //     Debug.Log("<color=magenta>PlayerPushBack! pushBack == </color>" + pushBack);
            }


        }
    }

    public void OnAvoidSoundEvent()
    {
        m_soundManager.PlaySE("AvoidancePlayer");
    }
}
