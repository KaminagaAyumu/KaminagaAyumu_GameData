using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AttackCollisionTest : MonoBehaviour
{
    public int m_damageAmount = 1;

    //“–‚½‚Á‚Ä‚¢‚é‚©
    private bool m_isHit = false;

    private void OnTriggerEnter(Collider other)
    {
        if (m_isHit) return;

        //Tag‚Å”»’è
        if (other.CompareTag("Enemy"))
        {
            BossHPController boss = other.GetComponentInParent<BossHPController>();
            if (boss != null)
            {
                boss.OnDamage(m_damageAmount);

                //“–‚½‚Á‚Ä‚¢‚é‚Ì‚Åƒtƒ‰ƒO‚ğtrue‚É
                m_isHit=true;

                //“–‚½‚Á‚Ä‚¢‚½‚ç‹…‚ğÁ‚·
                Destroy(gameObject);
            }
        }
    }
}
