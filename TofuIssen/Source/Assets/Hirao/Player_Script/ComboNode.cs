using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// コンボノード
/// </summary>
[SerializeField]
public class ComboNode
{
    public string animName;//アニメーションの名前
    public int attackPower;//攻撃力
    public float moveFrame;//突進する時間
    public float moveSpeed;//突進する速度
    public int[] nextLightAttack;//次の攻撃アニメーションのインデックスを管理する配列
    public int[] nextHeavyAttack;//次の攻撃アニメーションのインデックスを管理する配列
    public float seFrameRate;//SEを鳴らすタイミング
    public string seName;//SEの名前
    public int comboIndex = 0;//コンボの番号
}
[SerializeField]
public class ComboNodeList
{
    public List<ComboNode> comboNodes = new List<ComboNode>(); // 初期化しておく//コンボノードの配列
}
