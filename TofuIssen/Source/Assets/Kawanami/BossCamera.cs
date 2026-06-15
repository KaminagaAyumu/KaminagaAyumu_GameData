using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BossCamera : MonoBehaviour
{
    [Header("回転スピード")]
    [SerializeField]
    private float m_rotateSpeed = 10f;

    // 回転角度
    float m_angle;

    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void LateUpdate()
    {
        m_angle += m_rotateSpeed * Time.deltaTime;

        transform.rotation = Quaternion.Euler(0, m_angle, 0);
    }
}
