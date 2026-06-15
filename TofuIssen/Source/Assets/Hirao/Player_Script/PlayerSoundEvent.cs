using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerSoundEvent : MonoBehaviour
{
    [SerializeField] private SoundManager m_soundManager;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void OnAirSlashSoundEvent()
    {
        m_soundManager.PlaySE("PlayerAirSlash");
    }
}
