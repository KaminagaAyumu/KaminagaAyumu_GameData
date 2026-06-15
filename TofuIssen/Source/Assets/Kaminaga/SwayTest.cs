using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SwayTest : MonoBehaviour
{

    public float kSpeed = 2.0f; // —h‚ê‚é‘¬‚³
    public float kAmount = 5.0f; // —h‚ê‚é‘å‚«‚³

    private Quaternion m_firstRotation; // Å‰‚Ì‰ñ“]

    // Start is called before the first frame update
    void Start()
    {
        m_firstRotation = transform.rotation; // Å‰‚Ì‰ñ“]‚ğ•Û‘¶
    }

    // Update is called once per frame
    void Update()
    {
        float swish = Mathf.Sin(Time.time * kSpeed) * kAmount; // ŠÔ‚É‰‚¶‚½—h‚ê‚Ì’l‚ğŒvZ
        transform.rotation = m_firstRotation * Quaternion.Euler(0, 0, swish); // Å‰‚Ì‰ñ“]‚É—h‚ê‚ğ‰Á‚¦‚é
    }
}
