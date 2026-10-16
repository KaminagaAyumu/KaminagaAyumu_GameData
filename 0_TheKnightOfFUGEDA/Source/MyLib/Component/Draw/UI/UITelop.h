#pragma once
#include "../Drawable2D.h"
#include "../../Transform.h"
#include <string>

namespace MyLib
{
	class UITelop : public Drawable2D
	{
	public:

		explicit UITelop(DrawLayer layer = DrawLayer::UI);
		virtual ~UITelop() = default;

		void Init(std::weak_ptr<GameObject> parent) override;
		void Start() override;
		void Update() override;
		void End() override;
		void Draw() const override;
		bool IsAlive() const override { return m_isAlive; }

		void SetFontHandle(int handle) { m_fontHandle = handle; }
		void SetBandColor(unsigned int color) { m_bandColor = color; }
		void SetBandHeight(int height) { m_bandHeight = height; }

		// 演出のフレーム数は初期値を使ってメッセージを表示する
		void ShowMessage(const std::wstring& text);
		void ShowMessage(const std::wstring& text, int slideFrame, int overshootFrame, int holdFrame);

		bool IsSequenceFinished() const { return m_phase == Phase::Finished; }

	private:

		enum class Phase
		{
			Idle,
			SlideIn,
			Overshoot,
			Hold,
			SlideOut,
			Finished
		};

	private:

		std::weak_ptr<Transform> m_pTransform;

		int m_fontHandle;
		unsigned int m_bandColor;
		int m_bandHeight;

		std::wstring m_text;
		Phase m_phase;
		int m_phaseFrameCount;

		int m_slideFrame;
		int m_overshootFrame;
		int m_holdFrame;
		float m_overshootAmount;

		float m_textOffsetX;

		bool m_isAlive;

	private:

		void UpdateSlideIn();
		void UpdateOvershoot();
		void UpdateHold();
		void UpdateSlideOut();
	};
}


