#pragma once
#include "../Drawable2D.h"
#include "../../Transform.h"

namespace MyLib
{
	class UICountDown : public Drawable2D
	{
	public:

		explicit UICountDown(DrawLayer layer = DrawLayer::UI);
		virtual ~UICountDown() = default;

		void Init(std::weak_ptr<GameObject> parent) override;
		void Start() override;
		void Update() override;
		void End() override;
		void Draw() const override;
		bool IsAlive() const override { return m_isAlive; }

		void SetFontHandle(int handle) { m_fontHandle = handle; }
		void SetTextColor(unsigned int color) { m_textColor = color; }

		void StartCountDown(int startCount, int frameParCount, int startHoldFrame);

		bool IsFinished() const { return m_isFinished; }

	private:

		enum class CountPhase
		{
			Idle,
			Counting,
			ShowStart,
			Finished
		};

		std::weak_ptr<Transform> m_pTransform;

		CountPhase m_phase;

		int m_fontHandle;
		unsigned int m_textColor;

		int m_currentCount;
		int m_frameParCount;
		int m_frameCount;
		int m_startHoldFrame;

		bool m_isFinished;
		bool m_isAlive;
	};
}


