using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AnimationTest : MonoBehaviour
{

    private float m_currentRotZ;

    void LateUpdate()
    {
        Vector3 currentForward = transform.localRotation * Vector3.forward;

        // もしくは単純に現在のオイラー角のZ成分を取得
        m_currentRotZ = transform.localEulerAngles.z;

        // 2. 「ローカルのZ軸」を中心に、「Zマイルの回転量」だけを持つクォータニオンを【新規作成】する
        // これにより、XとYの回転成分は物理的に「存在しない」状態になります
        Quaternion pureZRotation = Quaternion.AngleAxis(m_currentRotZ, Vector3.forward);

        // 3. 完全にZ軸だけになった回転をボーンに強制上書きする
        transform.localRotation = pureZRotation;
    }
}
