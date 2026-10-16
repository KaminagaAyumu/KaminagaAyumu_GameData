#pragma once
#include "../Drawable2D.h"
#include "../../Transform.h"
#include "../../../../Geometry/Vector2.h"
#include "../../../../Geometry/Vector2Int.h"
#include <memory>

class File;

namespace MyLib
{
	/// <summary>
	/// 画像描画コンポーネント
	/// </summary>
	class UIImage : public Drawable2D
	{
	public:

		explicit UIImage(DrawLayer layer = DrawLayer::UI);

		virtual ~UIImage() = default;

		void Init(std::weak_ptr<GameObject> parent) override;
		void Start() override;
		void Update() override;
		void End() override;

		bool IsAlive() const override { return m_isAlive; }

		void SetImageFile(std::shared_ptr<File> pFile) { m_pImageFile = pFile; }
		void SetFrameFile(std::shared_ptr<File> pFile) { m_pFrameFile = pFile; }

		void Draw() const override;

		/// <summary>
		/// 表示する拡大率をセット
		/// </summary>
		/// <param name="scale">拡大率</param>
		void SetScale(const Vector2& scale) { m_scale = scale; }

		void SetRotation(float radian) { m_rotation = radian; }

		float GetRotation() { return m_rotation; }

		/// <summary>
		/// アニメーションをセットする
		/// </summary>
		/// <param name="animGraphSize">アニメーション1枚の画像サイズ</param>
		/// <param name="animNum">アニメーションの数</param>
		/// <param name="animFrame">アニメーションを更新するフレーム数</param>
		/// <param name="isLoop">ループするかどうか</param>
		void SetAnimation(const Vector2Int& animGraphSize, int animNum, int animFrame, bool isLoop);

		/// <summary>
		/// ループする背景をセットする
		/// </summary>
		/// <param name="loopFrame">ループするフレーム数</param>
		void SetLoopBackGround(int loopFrame);

		void StartAppearCenter(int appearFrame) override;

		/// <summary>
		/// 拡大縮小を行う
		/// </summary>
		/// <param name="pulseFrame"></param>
		/// <param name="pulseRange"></param>
		void StartPulse(int pulseFrame, float pulseRange);

		/// <summary>
		/// 拡縮を止める
		/// </summary>
		void StopPulse() { m_isPulsing = false; }

		/// <summary>
		/// 画像のタイプを設定
		/// </summary>
		enum class ImageType
		{
			Normal, // 通常描画
			Animation, // アニメーション描画
			LoopBackGround, // ループする背景画像
		};

	private:
		
		std::weak_ptr<Transform> m_pTransform;
		std::shared_ptr<File> m_pImageFile;
		std::shared_ptr<File> m_pFrameFile;

		// 存在フラグ
		bool m_isAlive;

		// スクロールする際の表示座標
		Vector2Int m_scrollPos;
		// 表示サイズ
		Vector2Int m_graphSize;
		// 表示スケール
		Vector2 m_scale;
		// 画像タイプ
		ImageType m_type;
		// アニメーション1枚のサイズ
		Vector2Int m_animGraphSize;
		// 回転(2Dのためこれ1つでよい)
		float m_rotation;
		// アニメーションの数
		int m_animNum;
		// アニメーションのフレーム
		int m_animFrame;
		// 背景をループさせる際のフレーム数
		int m_loopFrame;
		// アニメーションのループを行うかのフラグ
		bool m_isLoop;
		// アニメーション用のフレームカウンタ
		int m_animFrameCount;
		// アニメーションの進行率カウンタ
		int m_animCount;
		// 拡縮中かどうか
		bool m_isPulsing;
		// 拡縮するフレームカウンタ
		int m_pulseFrame;
		// 拡縮するスケール 
		float m_pulseRange;

	private:
		/// <summary>
		/// ループする背景を描画する際の処理
		/// </summary>
		void LoopBackGroundDraw() const;

	};

}

