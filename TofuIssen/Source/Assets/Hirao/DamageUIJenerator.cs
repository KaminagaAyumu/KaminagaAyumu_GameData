using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DamageUIJenerator : MonoBehaviour
{
    [SerializeField] UICode m_damagePrefab;// ダメージUIのプレハブ//生成する
    [SerializeField] Canvas m_canvas;//生成する場所


    readonly float m_lifeTime = 60.0f;//ダメージUIの寿命//0.5秒

    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.Space))
        {
            TakeDamage(Random.Range(10, 100));
        }

    }

    public void TakeDamage(int damage, Vector3 pos = new Vector3())
    {

        // ダメージUIを生成して、ダメージ数を表示する
        //ここにランダムな位置を指定して生成するコードも追加する
        //10の位が0の時はクリアなものを八つけるでもいい気がする

        Vector3 randomOffset = new Vector3(Random.Range(-150f, 150f), Random.Range(300, 400f), 0f);
        UICode damageUI = Instantiate(m_damagePrefab, m_canvas.transform);
        damageUI.transform.position = Camera.main.WorldToScreenPoint(pos) + randomOffset;
        damageUI.SetNumber(damage);
        damageUI.SetLifeTime(FrameToSeconds(m_lifeTime));

        //DestroyはUnity内部でタイマーを持つので,Time.deltaTimeをかける必要はない
        Destroy(damageUI.gameObject, FrameToSeconds(m_lifeTime));//0.5秒後にダメージUIを消す
    }
    private float FrameToSeconds(float frame, float fps = 60.0f)
    {
        return frame / fps;//フレーム数を秒数に変換する//60fpsの場合
    }

}
