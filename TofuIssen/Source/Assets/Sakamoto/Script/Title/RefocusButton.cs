using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class RefocusButton : MonoBehaviour
{
    //最後に選択されていたゲームオブジェクトを常に覚えておく
    private GameObject m_lastSlectedObject;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        //イベントシステムの現在選択されているオブジェクトがnullになっていたら
        //最後に選択していたオブジェクトを再設定する
        EventSystem eventSystem = EventSystem.current;

        if(eventSystem.currentSelectedGameObject == null)
        {
            //最後に選択していたオブジェクトを再設定する
            eventSystem.SetSelectedGameObject(m_lastSlectedObject);
        }
        else
        {
            //最後に選択されていたオブジェクトを取得
            m_lastSlectedObject = eventSystem.currentSelectedGameObject;
        }
    }
}
