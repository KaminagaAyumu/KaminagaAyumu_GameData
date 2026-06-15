using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MoveTestKaminaga : MonoBehaviour
{
    Vector3 pos;

    // Start is called before the first frame update
    void Start()
    {
        pos = transform.position;
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKey(KeyCode.UpArrow))
        {
            pos.z += 0.3f;
        }
        if (Input.GetKey(KeyCode.DownArrow))
        {
            pos.z -= 0.3f;
        }
        if (Input.GetKey(KeyCode.RightArrow))
        {
            pos.x += 0.3f;
        }
        if (Input.GetKey(KeyCode.LeftArrow))
        {
            pos.x -= 0.3f;
        }

        transform.position = pos;
    }
}
