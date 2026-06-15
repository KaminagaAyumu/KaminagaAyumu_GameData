using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class RedirectVolSlider : Slider
{
    [Header("スライダーに対応したボタン")]
    [SerializeField] private Button m_volButton;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public override void OnSelect(BaseEventData eventData)
    {
        //イベントシステムを取得
        EventSystem eventSystem = EventSystem.current;

        eventSystem.SetSelectedGameObject(null);
        eventSystem.SetSelectedGameObject(m_volButton.gameObject);
    }
}
