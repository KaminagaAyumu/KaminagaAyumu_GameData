using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MoveTest : MonoBehaviour
{
    float m_speed = 12.0f;

    void Start()
    {
        
    }

    void Update()
    {
        if (Input.GetKey(KeyCode.W))
        {
            transform.position += m_speed * transform.forward * Time.deltaTime;
        }
        if (Input.GetKey(KeyCode.S))
        {
            transform.position -= m_speed * transform.forward * Time.deltaTime;
        }
        if (Input.GetKey(KeyCode.D))
        {
            transform.position += m_speed * transform.right * Time.deltaTime;
        }
        if (Input.GetKey(KeyCode.A))
        {
            transform.position -= m_speed * transform.right * Time.deltaTime;
        }
    }
}
