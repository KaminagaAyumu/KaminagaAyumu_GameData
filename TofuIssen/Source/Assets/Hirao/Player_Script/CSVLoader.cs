using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CSVLoader : MonoBehaviour
{
    enum nodeIndex
    {
        animName = 0,
        attackPower = 1,
        moveFrame = 2,
        moveSpeed = 3,
        nextLightAttack = 4,
        nextHeavyAttack = 5,
        seFrameRate = 6,
        seName = 7,
        comboIndex = 8
    }

    public static ComboNodeList LoadComboNode(string fileName)
    {
        ComboNodeList nodeList = new ComboNodeList();


        //ResorceフォルダからCSVファイルを読み込む
        TextAsset csvFile = Resources.Load<TextAsset>(fileName);

        if(csvFile == null)
        {
            Debug.LogError("CSVファイルが見つかりません: " + fileName);
            return nodeList;
        }
        //CSVファイルの内容を行ごとに分割
        string[] lines = csvFile.text.Split('\n');
        //ヘッダー行をスキップ//3行目からデータが始まると仮定
        for (int i = 2;i < lines.Length;i++)
        {
            //行をトリムして空白を削除
            string line = lines[i].Trim();
            //,で行を分割して値を取得
            string[]values = line.Split(',');
            if(values.Length >= 7)
            {
                ComboNode node = new ComboNode
                {
                    //アニメーションの名前
                    animName = values[(int)nodeIndex.animName],
                    //攻撃力
                    attackPower = int.Parse(values[(int)nodeIndex.attackPower]),
                    //移動する時間
                    moveFrame = float.Parse(values[(int)nodeIndex.moveFrame]),
                    //移動する速さ
                    moveSpeed = float.Parse(values[(int)nodeIndex.moveSpeed]),
                    //軽攻撃ボタンを押したときに遷移できる攻撃のインデックス
                    nextLightAttack = ParseIntArray(values[(int)nodeIndex.nextLightAttack].Trim()),
                    //強攻撃ボタンを押したときに遷移できる攻撃のインデックス
                    nextHeavyAttack = ParseIntArray(values[(int)nodeIndex.nextHeavyAttack].Trim()),
                    //SEの再生するタイミング
                    seFrameRate = float.Parse(values[(int)nodeIndex.seFrameRate]),
                    //再生するSEの名前
                    seName = values[(int)nodeIndex.seName],
                    //エフェクトを出すかどうか
                    comboIndex = int.Parse(values[(int)nodeIndex.comboIndex])
                };
                nodeList.comboNodes.Add(node);
            }
        }

        return nodeList;
    }
    /// <summary>
    /// セミコロン区切りの文字列をint配列に変換する
    /// </summary>
    /// <param name="value">セミコロン区切りこみの文字列</param>
    /// <returns>int配列</returns>
    private static int[] ParseIntArray(string value)
    {
        //空文字列や空白のみの場合は空配列を返す
        if(string.IsNullOrWhiteSpace(value))
        {
            return new int[0];
        }
        //セミコロンで分割
        string[] parts = value.Split(';');
        int[] result = new int[parts.Length];
        for (int i = 0; i < parts.Length; i++)
        {
            //int型への変換を試みる//変換に失敗した場合false,成功した場合trueを返す//変換に成功した場合はparsedに変換後の値が格納される
            if (int.TryParse(parts[i].Trim(), out int parsed))
            {
                result[i] = parsed;
            }
            else
            {
                Debug.LogWarning($"int型への変換に失敗: '{parts[i]}' → デフォルト値 -1 を使用");
               result[i] = -1;//パースに失敗した場合は-1を設定
            }

        }
        return result;
    }
}
