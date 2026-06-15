using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerAttackTest : MonoBehaviour
{
    public GameObject attackPrefab; //Prefab
    public Transform attackPoint;   //UŒ‚‚ªo‚éˆÊ’u

    void Update()
    {
        //ZƒL[‚ª‰Ÿ‚³‚ê‚½‚ç
        if (Input.GetKeyDown(KeyCode.Z))
        {
            //UŒ‚ŠÖ”‚ğÀs
            Attack();
        }
    }

    void Attack()
    {
        //UŒ‚”»’è‚ğ¶¬
        GameObject obj=Instantiate(attackPrefab, attackPoint.position, attackPoint.rotation);

        //0.5•bŒã‚ÉÁ‚·
        Destroy(obj, 0.5f);
    }
}
