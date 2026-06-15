using System.Collections;
using System.Collections.Generic;
using System.Net.Http.Headers;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

public class SceneManagerKaminaga : MonoBehaviour
{
    // シーンマネージャーのインスタンスを作成(シングルトン)
    //public static SceneManagerKaminaga Instance { get; private set; }

    [SerializeField] private SceneName m_currentSceneName;  // 現在のシーンの名前
    [SerializeField] private GameObject m_fadeCanvas;       // フェードで使用する画像を使うCanvas
    [SerializeField] private Image m_fadeImage;             // フェードで使用する画像イメージ
    [SerializeField] private Image m_loadingSliderBack;     // ロード中の進捗バー背景
    [SerializeField] private Image m_loadingSlider;         // ロード中の進捗バー
    [SerializeField] private Image m_loadingImage;          // ロード中画面
    [SerializeField] private float m_fadeDuration = 1.0f;   // フェードの進行スピード

    private float m_displayProgress = 0f;                   // 画面に進捗率を表示するための値

    private bool m_isLoading = false;                       // ロード中フラグ(trueの時はさらにシーンがロードされないようにする)

    private GameObject m_selectedObject;                    // 選択されたオブジェクト

    //音が重ならないようにGetterを作る
    public bool GetIsLoading()
    {
        return m_isLoading;
    }


    [Header("SoundManager")]
    [SerializeField] private SoundManager m_soundManager;

    void Start()
    {
        // フェードアウトしてからゲームを始める
        StartCoroutine(Fade(0.0f));
    }

    private void Awake()
    {
        //イベントにメソッドを登録する
        SceneManager.sceneLoaded += OnSceneLoaded;
    }

    private void OnDestroy()
    {
        //登録したメソッドを解除する
        SceneManager.sceneLoaded -= OnSceneLoaded;
    }

    private void OnSceneLoaded(Scene scene, LoadSceneMode mode)
    {
        //現在のシーンによって鳴らすBGMを変える
        // 次のシーンのBGMを再生する
        m_soundManager.FadeInBGM(scene.name + "BGM");
    }

    void Update()
    {
        // ロード中にシーン遷移しないようにする
        if (m_isLoading)
        {
            // 現在使用しているEventSystemを取得する
            EventSystem currentEvent = EventSystem.current;

            // 選択されたボタンが存在する場合
            if(m_selectedObject != null)
            {
                // 選択されているボタンが再選択されないようにする
                m_selectedObject.GetComponent<Button>().interactable = false;
            }

            // 選択されているオブジェクトを最後に選択されたものに固定する
            currentEvent.SetSelectedGameObject(m_selectedObject);
            return;
        }

        // Escキーが押されたらゲームを終了する
        if (Input.GetKeyDown(KeyCode.Escape))
        {
#if UNITY_EDITOR
            // エディター終了
            UnityEditor.EditorApplication.isPlaying = false;
#else
        // ゲーム終了
        Application.Quit();
#endif
        }

#if UNITY_EDITOR
        // デバッグ用
        if (Input.GetKeyDown(KeyCode.E))
        {
            m_currentSceneName = SceneName.Title;
            LoadScene((int)m_currentSceneName);
        }
        if (Input.GetKeyDown(KeyCode.R))
        {
            m_currentSceneName = SceneName.Game;
            LoadScene((int)m_currentSceneName);
        }
        if (Input.GetKeyDown(KeyCode.T))
        {
            m_currentSceneName = SceneName.Result;
            LoadScene((int)m_currentSceneName);
        }
#endif
    }

    public void LoadScene(int sceneName)
    {
        if (m_isLoading)
        {
            Debug.Log("シーン遷移中");
            return;
        }

        StartCoroutine(LoadSceneASync((SceneName)sceneName));
    }

    public void GameEnd()
    {
        // フェードアウトしてからゲームを終了する
        StartCoroutine(GameEnd(1.0f));
    }

    /// <summary>
    /// 非同期読み込みでシーンをロードする
    /// </summary>
    /// <param name="sceneName">ロードするシーン名</param>
    /// <returns></returns>
    IEnumerator LoadSceneASync(SceneName sceneName)
    {
        // シーン切り替えが重複しないようにフラグをtrueにする
        m_isLoading = true;
        
        // 現在使用しているEventSystemを取得する
        EventSystem currentEvent = EventSystem.current;

        // 選択されているオブジェクトを現在のものに固定する
        m_selectedObject = currentEvent.currentSelectedGameObject;

        // 現在再生中のBGMをフェードアウトさせる
        m_soundManager.FadeOutBGM(2.0f);

        // フェード処理で画面全体を黒にフェードインしていく
        yield return StartCoroutine(Fade(1.0f));

        // ロード時の画像をアクティブ状態にする
        m_loadingImage.gameObject.SetActive(true);

        // ロード時のスライダー背景をアクティブ状態にする
        m_loadingSliderBack.gameObject.SetActive(true);

        // ロード時のスライダーをアクティブ状態にする
        m_loadingSlider.gameObject.SetActive(true);

        yield return StartCoroutine(Fade(0.0f));

        // シーンを非同期でロードする
        // ロードするシーンのビルド番号に変換
        int sceneIndex = (int)sceneName;
        // ビルド番号に応じたシーンをロードする
        AsyncOperation operation = SceneManager.LoadSceneAsync(sceneIndex);

        m_currentSceneName = sceneName;

        // シーンの切り替え状態をfalseにする(ロード中にシーンを切り替えないようにする)
        // ロードが90%を超えたらシーンが切り替わる
        operation.allowSceneActivation = false;

        // 実際に表示する値をリセット
        m_displayProgress = 0.0f;  

        // シーンの切り替えが終わっていない時の処理
        while (!operation.isDone)
        {
            // シーンのロードの進捗率を0~1にする
            // (デフォルトではprogressの最大値が0~0.9になっている)
            float rate = Mathf.Clamp01(operation.progress / 0.9f);

            // 時間をかけて実際の値に近づける
            m_displayProgress = Mathf.MoveTowards(m_displayProgress, rate, Time.deltaTime * 5.0f);
            // 実際に表示するための値に経過割合を代入
            //m_displayProgress = rate;

            // ロード進捗バーを更新
            m_loadingSlider.fillAmount = m_displayProgress;

            // ロードが完了したら
            if (m_displayProgress >= 1f)
            {
                // 1秒間待つ(ロードが速すぎたときでもこの画面を最低1秒は見せるため)
                yield return new WaitForSeconds(1.0f);
                // シーンの切り替えフラグをtrueにする
                operation.allowSceneActivation = true;
            }

            yield return null;
        }

        // ロード時のスライダー背景を非アクティブ状態にする
        m_loadingSliderBack.gameObject.SetActive(true);

        // ロード時のスライダーを非アクティブ状態にする
        m_loadingSlider.gameObject.SetActive(false);

        // フェード処理で画面全体を黒からフェードアウトしていく
        yield return StartCoroutine(Fade(1.0f));
        yield return StartCoroutine(Fade(0.0f));

        // シーンのロード中のフラグをfalseにしてシーン切り替え可能にする
        m_isLoading = false;
    }

    private IEnumerator Fade(float targetAlpha)
    {
        // 画像の開始アルファ値を取得
        float startAlpha = m_fadeImage.color.a;
        // 経過時間をリセット
        float time = 0;

        // 経過時間がフェード最大時間になるまで行う処理
        while (time < m_fadeDuration)
        {
            // 時間をカウント
            time += Time.deltaTime;
            // アルファ値をターゲットに向かって線形補間していく
            float alpha = Mathf.Lerp(startAlpha, targetAlpha, time / m_fadeDuration);

            // フェードさせる画像の色を取得
            Color color = m_fadeImage.color;
            // 色のアルファ値をフェード用のものに設定
            color.a = alpha;
            // アルファ値を実際の画像に適用
            m_fadeImage.color = color;

            yield return null;
        }

        // 念のため指定されたアルファ値に強制的に変更する
        Color finalColor = m_fadeImage.color;
        finalColor.a = targetAlpha;
        m_fadeImage.color = finalColor;
    }

    private IEnumerator GameEnd(float targetAlpha)
    {
        // 画像の開始アルファ値を取得
        float startAlpha = m_fadeImage.color.a;
        // 経過時間をリセット
        float time = 0;

        // UI操作ができないようにフラグをtrueにする
        m_isLoading = true;

        // 現在使用しているEventSystemを取得する
        EventSystem currentEvent = EventSystem.current;

        // 選択されているオブジェクトを現在のものに固定する
        m_selectedObject = currentEvent.currentSelectedGameObject;

        // 経過時間がフェード最大時間になるまで行う処理
        while (time < m_fadeDuration)
        {
            // 時間をカウント
            time += Time.deltaTime;
            // アルファ値をターゲットに向かって線形補間していく
            float alpha = Mathf.Lerp(startAlpha, targetAlpha, time / m_fadeDuration);

            // フェードさせる画像の色を取得
            Color color = m_fadeImage.color;
            // 色のアルファ値をフェード用のものに設定
            color.a = alpha;
            // アルファ値を実際の画像に適用
            m_fadeImage.color = color;

            yield return null;
        }

        // 念のため指定されたアルファ値に強制的に変更する
        Color finalColor = m_fadeImage.color;
        finalColor.a = targetAlpha;
        m_fadeImage.color = finalColor;

#if UNITY_EDITOR
        // エディター終了
        UnityEditor.EditorApplication.isPlaying = false;
#else
        // ゲーム終了
        Application.Quit();
#endif
    }

}