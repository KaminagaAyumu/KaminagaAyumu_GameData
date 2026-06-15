 using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class CursorController : MonoBehaviour
{
    //カーソルのUI座標
    private RectTransform m_cursorPos;

    //カーソルを左にずらすときの値
    [SerializeField] private float m_cursorOffsetX = 10.0f;

    //カーソルを動かすときの速さ
    [SerializeField] private float m_cursorSpeed = 4.0f;

    // Start is called before the first frame update
    void Start()
    {
        //RectTransformコンポーネントを取得
        m_cursorPos = GetComponent<RectTransform>();
    }

    // Update is called once per frame
    void Update()
    {
        //イベントシステムを取得
        EventSystem eventSystem = EventSystem.current;
        //現在選択されているゲームオブジェクトを取得
        GameObject currentSelectedObject = eventSystem.currentSelectedGameObject;
        //現在選択されているゲームオブジェクトの位置を取得
        if (currentSelectedObject != null)
        {
            RectTransform selectedObjectRT = currentSelectedObject.GetComponent<RectTransform>();
            //選択しているボタンの横幅の半分
            float halfWidth = selectedObjectRT.rect.width / 2;

            Vector3 pos = new Vector3(selectedObjectRT.position.x - m_cursorOffsetX - halfWidth, selectedObjectRT.position.y, 0.0f);
            m_cursorPos.position = Vector3.Lerp(m_cursorPos.position, pos, m_cursorSpeed * Time.deltaTime);
        }
    }
}
