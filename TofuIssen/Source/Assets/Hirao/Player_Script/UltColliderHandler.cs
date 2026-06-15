using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UltColliderHandler : MonoBehaviour
{

    //Playerを持つ
    private Player m_parentOwner;



    // Start is called before the first frame update
    void Start()
    {
        //親オブジェクトからPlayerコンポーネントを取得
        m_parentOwner = GetComponentInParent<Player>();
        if (m_parentOwner == null)
        {
            Debug.LogError("HitColliderHandler: Player component not found in parent.");
        }
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    private void OnTriggerStay(Collider other)
    {
        //Playerコンポーネントが見つからない場合は早期リターン
        if (m_parentOwner == null) return;

        if (other.CompareTag("Enemy"))
        {
            m_parentOwner.UltimateHit(other);
        }

    }
}
