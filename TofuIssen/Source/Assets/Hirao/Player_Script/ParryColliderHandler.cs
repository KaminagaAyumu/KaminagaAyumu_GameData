using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ParryColliderHandler : MonoBehaviour
{
    //Playerを持つ
    private Player m_parentOwner;

    // Start is called before the first frame update
    void Start()
    {
        //親オブジェクトからPlayerコンポーネントを取得
        m_parentOwner = GetComponentInParent<Player>();
        if(m_parentOwner == null)
        {
            Debug.LogError("ParryColliderHandler: Player component not found in parent.");
        }

    }

    // Update is called once per frame
    void Update()
    {
        
    }
    private void OnTriggerEnter(Collider other)
    {
        //Playerコンポーネントが見つからない場合は早期リターン
        if (m_parentOwner == null) return;
        
        if(other.CompareTag("EnemyAttack"))
        {
            //親オブジェクトを取得
            Collider parent = other.GetComponentInParent<Collider>();

            //PlayerのParryCheck関数を呼び出す
            m_parentOwner.ParryHit(parent);
        }
        
    }

}
