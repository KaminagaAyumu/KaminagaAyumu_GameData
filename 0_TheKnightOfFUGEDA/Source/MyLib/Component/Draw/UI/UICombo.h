#pragma once
#include "../Drawable2D.h"
#include "../../Transform.h"
#include <string>

namespace MyLib
{
	class UICombo : public Drawable2D
	{
	public:

		explicit UICombo(DrawLayer layer = DrawLayer::UI);
		virtual ~UICombo() = default;

		void Init(std::weak_ptr<GameObject> parent) override;
		void Start() override;
		void Update() override;
		void End() override;
		void Draw() const override;
		bool IsAlive() const override { return m_isAlive; }

		void SetFontHandle(int handle) { m_fontHandle = handle; }
		void SetTextColor(unsigned int color) { m_textColor = color; }
		void SetText(std::wstring text) { m_text = text; }

		void SetParam(int count, float remainRate);

	private:

		std::weak_ptr<Transform> m_pTransform;

		std::wstring m_text;

		int m_fontHandle;
		unsigned int m_textColor;

		bool m_isAlive;

		int m_count;
		float m_remainRate;
	};
}



