using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

public class MirrorPlacementTool : EditorWindow
{
    [MenuItem("Tools/オブジェクト反転ツール")]
    public static void ShowWindow()
    {
        GetWindow<MirrorPlacementTool>("Object Mirror Tool");
    }

    private void OnGUI()
    {
        GUILayout.Label("選択中のオブジェクトをX軸基準で反転複製します", EditorStyles.boldLabel);

        if(GUILayout.Button("右へ反転複製"))
        {
            MirrorSelectedObject();
        }
    }

    private void MirrorSelectedObject()
    {
        GameObject selected = Selection.activeGameObject;
        if (selected == null)
        {
            Debug.LogWarning("オブジェクトが選択されていません。");
            return;
        }

        // 複製を作成
        GameObject cloned = Instantiate(selected, selected.transform.parent);
        cloned.name = selected.name + "_Mirrored";

        // 位置の反転(X座標の符号を反転)
        Vector3 pos = selected.transform.localPosition;
        pos.x = -pos.x; // 例:-5.5 -> 5.5
        cloned.transform.localPosition = pos;

        // 回転の反転(Y軸とZ軸の符号を反転)
        Vector3 rot = selected.transform.localEulerAngles;
        rot.y = -rot.y;
        rot.z = -rot.z;
        cloned.transform.localEulerAngles = rot;

        // 変更をUnityエディタの保存対象にする
        Undo.RegisterCreatedObjectUndo(cloned, "Mirror Object");
    }

}
