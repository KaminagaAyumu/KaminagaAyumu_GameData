#pragma once
#include "../Drawable2D.h"
#include "../../Transform.h"
#include <string>

namespace MyLib
{
	class UIText : public Drawable2D
	{
	public:
		/// <summary>
		/// 文字の寄せかた
		/// </summary>
		enum class AlignmentType
		{
			Left,
			Right,
			Center
		};
	public:

		explicit UIText(DrawLayer layer = DrawLayer::UI);
		virtual ~UIText() = default;

		void Init(std::weak_ptr<GameObject> parent) override;
		void Start() override;
		void Update() override;
		void End() override;
		void Draw() const override;
		bool IsAlive() const override { return m_isAlive; }

		void SetFontHandle(int handle) { m_fontHandle = handle; }
		void SetTextColor(unsigned int color) { m_textColor = color; }
		void SetText(std::wstring text) { m_text = text; }

		void SetAlignment(AlignmentType type) { m_alignmentType = type; }
	private:

		std::weak_ptr<Transform> m_pTransform;

		std::wstring m_text;

		int m_fontHandle;
		unsigned int m_textColor;

		bool m_isAlive;

		// 文字の寄せかたを取得
		AlignmentType m_alignmentType;
	};
}


