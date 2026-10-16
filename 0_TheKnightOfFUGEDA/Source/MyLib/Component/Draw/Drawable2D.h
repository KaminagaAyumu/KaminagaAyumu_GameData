#pragma once
#include "Drawable.h"

namespace MyLib
{

	/// <summary>
	/// 2D描画基底クラス
	/// ここからUIなどを作っていく
	/// </summary>
	class Drawable2D abstract : public Drawable
	{
	public:

		enum class UILayer : uint8_t
		{
			BackGround,		// 背景
			Normal,			// 通常
			Front,			// 前面
			Max				// レイヤーの最大数
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
			BlinkOut, // 点滅して消える
			AppearCenter, // 中央から出てくる
			CloseCenter, // 中央から消える
		};

	public:

		explicit Drawable2D(DrawLayer layer);
		virtual ~Drawable2D() = default;

		void SetUILayer(UILayer uiLayer) { m_uiLayer = uiLayer; }

		UILayer GetUILayer() const { return m_uiLayer; }

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

		float GetBlinkAlphaRate()const;

		/// <summary>
		/// 点滅を開始する
		/// </summary>
		/// <param name="blinkFrame">点滅1周のフレーム数</param>
		void StartBlinking(int blinkFrame);

		/// <summary>
		/// 点滅を停止し、通常の状態に戻す
		/// </summary>
		void StopBlinking();

		/// <summary>
		/// 数フレーム点滅してから消える動きを行う
		/// </summary>
		/// <param name="blinkFrame">点滅するフレーム数</param>
		/// <param name="isAfterDelete">終わった後に消去するか</param>
		void StartBlinkOut(int blinkFrame, bool isAfterDelete);

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

		bool UpdateFade();

		float GetFadeAlphaRate() const;

		bool UpdateAppearState();

		float GetAppearRate()const;

	protected:

		// UIのレイヤー
		UILayer m_uiLayer;

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

}


