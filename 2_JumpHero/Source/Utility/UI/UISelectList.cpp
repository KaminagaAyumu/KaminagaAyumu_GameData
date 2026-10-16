#include "UISelectList.h"
#include "../StringFunction.h"
#include "DxLib.h"

namespace
{
	constexpr int kFirstCreateList = 1; // リストを生成した際のサイズ

	constexpr int kDefaultPaddingX = 16; // リストの左右端からテキストまでの余白の初期値
	constexpr int kDefaultPaddingY = 10; // リストの上下端からテキストまでの余白の初期値
	constexpr int kDefaultItemSpacing = 100; // テキストとテキストの間の余白の初期値

	constexpr int kWindowAlpha = 200; // テキストを表示する範囲の透明度

	constexpr int kOutlineMargin = 2; // 影として使うテキストのずらす値
	
	constexpr float kTextWaveSpeed = 0.15f; // テキストの動く速さ(フレームカウンタを参照)
	constexpr float kTextWaveSize = 4.0f; // テキストを動かす際の動く範囲
	constexpr int kStringOneSize = 1; // 1文字のサイズを示す

	constexpr unsigned int kDefaultBgColor = 0xff2200; // 背景をBoxで表示する際の色
	constexpr unsigned int kSelectTextColor = 0xff00ff; // 選択されている際のテキストの色
}

UISelectList::UISelectList() : 
	m_pos{},
	m_size{},
	m_fontHandle(-1),
	m_color(0),
	m_itemSpacing(kDefaultItemSpacing),
	m_frameCount(0),
	m_cursor(0),
	m_frameHandle(-1),
	m_fillHandle(-1),
	m_paddingX(0),
	m_policy(OffsetPolicy::Left),
	m_isDialogMode(false)
{
	m_items.clear();
}

UISelectList::~UISelectList()
{
}

void UISelectList::AddOption(const std::string& text, std::function<void()> onSelect)
{
	m_items.push_back(OptionItem{ text, std::move(onSelect) });

	// 初回追加時にカーソルを0にする
	if (m_items.size() == kFirstCreateList)
	{
		m_cursor = 0;
	}
}

void UISelectList::Init(int fontHandle, const Size& size, const Position2& pos)
{
	m_fontHandle = fontHandle;
	m_size = size;
	m_pos = pos;
}

void UISelectList::Update()
{
	m_frameCount++;
}

void UISelectList::Draw() const
{
	if (m_isDialogMode)
	{
		// はいといいえを表示する
	}
	else
	{
		// テキストとカーソルをサイズの範囲内に描画する
	// テキストは左詰め
	// 上から順に均等に項目を並べる
	// 選択している項目が分かるようにするが、やり方(大きくしたり少し右寄りにするなど)は未定

		const int halfW = m_size.width / 2;
		const int halfH = m_size.height / 2;
		const int top = static_cast<int>(m_pos.y) - halfH;
		const int bottom = static_cast<int>(m_pos.y) + halfH;
		const int left = static_cast<int>(m_pos.x) - halfW;
		const int right = static_cast<int>(m_pos.x) + halfW;

		// 背景を透過する
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, kWindowAlpha);
		// 背景画像があるなら画像で、ないならBoxで背景を描画
		if (m_fillHandle != -1)
		{
			DrawExtendGraph(left, top, right, bottom, m_fillHandle, true); // ウィンドウの背景を描画
		}
		else
		{
			DrawBox(left, top, right, bottom, kDefaultBgColor, true);
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		// 枠画像があるなら枠を表示
		if (m_frameHandle != -1)
		{
			DrawExtendGraph(left, top, right, bottom, m_frameHandle, true); // ウィンドウの背景を描画
		}

		// ここはデフォルトから変えられるようにするかもしれない
		const int paddingX = kDefaultPaddingX;

		// テキストを描画する範囲
		const int areaW = m_size.width - paddingX * 2;
		
		// フォントのサイズを取得
		const int fontSize = GetFontSizeToHandle(m_fontHandle);
		
		// 項目を並べるY座標
		int y = top + kDefaultPaddingY;

		// 項目の数だけ並べていく処理
		for (int i = 0; i < static_cast<int>(m_items.size()); i++)
		{
			// カーソルがこの項目にあっているか
			const bool isSelected = (i == m_cursor);
			// テキストをワイド文字列に変換しておく
			const std::wstring wText = StringFunction::WStringFromString(m_items[i].text);
			// テキストの幅を取得
			const int lineW = GetDrawStringWidthToHandle(wText.c_str(), static_cast<int>(wText.size()), m_fontHandle);

			// 表示するX座標を確認
			int x = left + paddingX;

			// 文字の幅がテキストを描画する範囲内に収まっているとき
			if (lineW <= areaW)
			{
				// X座標は中央ぞろえにする
				x = left + paddingX + (areaW - lineW) / 2;
			}

			// 垂直位置の基準Y座標
			int baselineY = y + (kDefaultItemSpacing - fontSize) / 2;

			// 文字を1文字ずつ動かす際に進めるための値
			int advance = 0;
			// 文字が波打つスピードを調整
			float t = m_frameCount * kTextWaveSpeed;

			// 文字の数分ループ
			for (int c = 0; c < static_cast<int>(wText.size()); c++)
			{
				// 文字の内容を取得
				wchar_t ch = wText[c];
				// ワイド文字列にする
				std::wstring one(kStringOneSize, ch);
				// この文字の幅を取得
				int cw = GetDrawStringWidthToHandle(one.c_str(), kStringOneSize, m_fontHandle);

				// テキストが波打つためのオフセット
				int yOffset = 0;
				// 選択中の場合
				if (isSelected)
				{
					// オフセットが波打つ感じにする
					yOffset = static_cast<int>(std::sin(t - c * 0.5f) * kTextWaveSize);
				}

				// 影にするテキストを描画
				DrawStringToHandle(x + advance + kOutlineMargin, baselineY + kOutlineMargin + yOffset, one.c_str(), GetColor(0, 0, 0), m_fontHandle);
				// 実際のテキストを描画
				// 選択中の場合は色を変える
				DrawStringToHandle(x + advance, baselineY + yOffset, one.c_str(), isSelected ? 0xffffff : kSelectTextColor, m_fontHandle);

				// 文字の描画X座標をこの文字の幅分進める
				advance += cw;
			}
			
			// 次の項目のy座標に進める
			y += kDefaultItemSpacing;
			// y座標がこれ以上進めない場合処理を抜ける
			if (y > bottom - kDefaultPaddingY) break;
		}
	}

}

bool UISelectList::IsAlive() const
{
	// 存在判定を変えられるようにしなければならない
	return true;
}

void UISelectList::SetHandle(int frameHandle, int fillHandle)
{
	m_frameHandle = frameHandle;
	m_fillHandle = fillHandle;
}

void UISelectList::MoveCursor(int dir)
{
	// 選択肢がない場合
	if (m_items.empty() || dir == 0)
	{
		// カーソルを動かさない
		return;
	}
	
	// 現在のカーソルから値の数分動かす
	m_cursor = (m_cursor + dir + static_cast<int>(m_items.size())) % static_cast<int>(m_items.size());
}

void UISelectList::TriggerSelect()
{
	// 選択肢がない場合
	if (m_items.empty())
	{
		return;
	}

	// 選択肢が選択できるものである場合
	if (0 <= m_cursor && m_cursor < static_cast<int>(m_items.size()))
	{
		// 選択された際の処理が存在する場合
		if (m_items[m_cursor].onSelect)
		{
			// 選択された処理を行う
			m_items[m_cursor].onSelect();
		}
	}
}

void UISelectList::SetDialogMode(const std::string& text)
{
	m_dialogTitle = text;
	m_isDialogMode = true;
}