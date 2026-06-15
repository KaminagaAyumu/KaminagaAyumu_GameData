using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UltEffectController : MonoBehaviour
{
    [Header("アニメーション制御")]
    [SerializeField] private AnimationCurve m_speedCurve;

    private Transform m_target;                 // 向かうターゲット
    private Vector3 m_startPos;                 // 開始位置
    private Vector3 m_controlPos;               // 制御する座標
    private float m_elapsedTime;                // 経過時間
    private float m_moveDurationTime = 1.0f;    // 目標位置まで到達する時間
    private System.Action<int> OnComplete;      // 完了したときに呼ぶイベント
    private int m_increaseGaugeParam;           // ウルトゲージを増やす値

    public void Init(Transform target, System.Action<int> onComplete, int increaseGaugeParam)
    {
        Debug.Log("Init初期化");
        m_startPos = transform.position;

        m_target = target;          // ターゲットを設定
        OnComplete = onComplete;    // 終了したときの処理
        m_increaseGaugeParam = increaseGaugeParam; // ゲージを増やす量
    }

    // Start is called before the first frame update
    void Start()
    {
        Debug.Log("Start初期化");
        //m_startPos = transform.position;
        m_elapsedTime = 0.0f;               // 時間を初期化

        // オフセットを設定
        // ランダムな円の範囲に設定
        Vector3 offset = Random.insideUnitSphere * 14.0f;
        offset.y = Mathf.Abs(offset.y);
        m_controlPos = m_startPos + offset;
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        m_elapsedTime += Time.deltaTime;
        float normalizedTime = m_elapsedTime / m_moveDurationTime; // 経過時間の割合を取得

        // エフェクトがターゲットに到達したら
        if(normalizedTime >= 1.0f)
        {
            // 終了時に呼ぶ処理を設定
            OnComplete?.Invoke(m_increaseGaugeParam);

            Debug.Log("消えた");

            Destroy(this.gameObject);
        }

        float evaluateTime = m_speedCurve.Evaluate(normalizedTime);

        Vector3 endPos = m_target.position;
        Vector3 m0 = Vector3.Lerp(m_startPos, m_controlPos, evaluateTime);
        Vector3 m1 = Vector3.Lerp(m_controlPos, endPos, evaluateTime);

        transform.position = Vector3.Lerp(m0, m1, evaluateTime);
    }
}
