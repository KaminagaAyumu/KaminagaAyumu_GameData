using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ParryVirtualCamera : MonoBehaviour
{
    [Header("オフセット")]
    [SerializeField]
    float m_offsetX = 0f;
    [SerializeField]
    float m_offsetY = 0f;
    [SerializeField]
    float m_offsetZ = 0f;

    // Start is called before the first frame update
    void Start()
    {
        transform.localPosition = new Vector3(m_offsetX, m_offsetY, m_offsetZ);
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
