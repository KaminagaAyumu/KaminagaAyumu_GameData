#pragma once
/// <summary>
/// UI関連の基底クラス
/// </summary>
class UIBase abstract
{
public:
	UIBase();
	virtual ~UIBase() = default; // 処理なし
	virtual void Init()abstract;
	virtual void End()abstract;
	virtual void Update()abstract;
	virtual void Draw()const abstract;

	/// <summary>
	/// UIが存在しているかの判定を行う
	/// </summary>
	/// <returns>true : 存在している false : 存在しない</returns>
	virtual bool IsAlive()const { return true; }

	/// <summary>
	/// UIの表示状態の判定を行う
	/// </summary>
	/// <returns>true : 表示している false : 表示していない</returns>
	bool IsActive() const { return m_isActive; }

	/// <summary>
	/// UIの表示状態を設定する
	/// </summary>
	/// <param name="isActive">true : 表示する false : 表示しない</param>
	void SetActive(bool isActive) { m_isActive = isActive; }

	/// <summary>
	/// UIの描画順
	/// 上から順に描画する
	/// </summary>
	enum class Layer : int
	{
		BackGround,
		Tutorial,
		Game,
		InGame,
		PauseBackGround,
		Pause,
		Max
	};

	/// <summary>
	/// フェードの状態
	/// </summary>
	enum class FadeState
	{
		FadeIn,
		Normal,
		FadeOut
	};

	/// <summary>
	/// 描画などの状態
	/// </summary>
	enum class UIState
	{
		Normal, // 通常
		Blinking, // 点滅
		AppearCenter, // 中央から出てくる
		CloseCenter, // 中央から消える
	};

	/// <summary>
	/// UIのレイヤーをセットする
	/// </summary>
	/// <param name="layer">指定レイヤー</param>
	void SetLayer(Layer layer) { m_layer = layer; }

	/// <summary>
	/// UIのレイヤーを取得する
	/// </summary>
	/// <returns>UIのレイヤー</returns>
	Layer GetLayer()const { return m_layer; }

	/// <summary>
	/// フェードインを開始する
	/// </summary>
	/// <param name="fadeFrame">フレーム数</param>
	void StartFadeIn(int fadeFrame);

	/// <summary>
	/// フェードアウトを開始する
	/// </summary>
	/// <param name="fadeFrame">フレーム数</param>
	/// <param name="isAfterDelete">終わった後に消去するか</param>
	void StartFadeOut(int fadeFrame, bool isAfterDelete);

	/// <summary>
	/// フェードかどうかを判定する
	/// </summary>
	/// <returns></returns>
	bool IsFade() const;

	/// <summary>
	/// 動いているかどうかを判定する
	/// </summary>
	/// <returns></returns>
	bool IsMoving() const;

	/// <summary>
	/// 点滅を開始する
	/// </summary>
	/// <param name="blinkFrame">点滅1周のフレーム数</param>
	void StartBlinking(int blinkFrame);

	/// <summary>
	/// 中央から出てくる動きを行う
	/// </summary>
	/// <param name="appearFrame">出現が終わるまでのフレーム</param>
	virtual void StartAppearCenter(int appearFrame);

	/// <summary>
	/// 中央に向かって消える動きを行う
	/// </summary>
	/// <param name="closeFrame">消えるのが終わるまでのフレーム</param>
	/// <param name="isAfterDelete">終わった後に消去するか</param>
	void StartCloseCenter(int closeFrame, bool isAfterDelete);

protected:

	// UIのレイヤー
	Layer m_layer;

	// フェード状態
	FadeState m_fadeState;

	// 描画の状態
	UIState m_uiState;

	// フェード時間のフレーム
	int m_fadeFrame;

	// フレームのカウンタ
	int m_frameCount;

	// 点滅1周のフレーム
	int m_blinkFrame;

	// 出現が終わるまでのフレーム
	int m_appearFrame;

	// UIの更新のフレームカウンタ
	int m_uiUpdateFrameCount;

	// UIを表示しているかどうか
	bool m_isActive;

	// 終了した後に消去するか
	bool m_isAfterDelete;
};

