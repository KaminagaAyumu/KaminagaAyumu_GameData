using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class SliderPointerUP : MonoBehaviour,IPointerUpHandler
{
    [Header("サウンドマネージャー")]
    [SerializeField] private SoundManager m_soundManager;

    [Header("プレビューで鳴らす音の名前")]
    [SerializeField] private string m_name;
    

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void OnPointerUp(PointerEventData eventData)
    {
        //音を鳴らす
        m_soundManager.PlaySE(m_name);
    }
}
