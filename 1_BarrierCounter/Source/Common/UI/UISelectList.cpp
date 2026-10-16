#include "UISelectList.h"
#include "DxLib.h"
#include <cassert>

namespace
{
	constexpr int kFirstCreateList = 1; // リストを生成した際のサイズ
	constexpr int kDialogItemNum = 2; // ダイアログの際に表示するテキストの数

	constexpr int kDefaultPaddingX = 16; // リストの左右端からテキストまでの余白の初期値
	constexpr int kDefaultPaddingY = 10; // リストの上下端からテキストまでの余白の初期値
	constexpr int kDialogTitlePaddingY = 10; // リストの上端からダイアログの見出しまでの余白の初期値
	constexpr int kDialogPaddingX = 80; // リストの描画する範囲の左右端から選択肢までの余白の初期値
	constexpr int kDialogPaddingY = 40; // リストの上端からダイアログの見出しまでの余白の初期値
	constexpr int kDefaultItemSpacing = 100; // テキストとテキストの間の余白の初期値

	constexpr int kWindowAlpha = 150; // テキストを表示する範囲の透明度

	constexpr int kOutlineMargin = 2; // 影として使うテキストのずらす値

	constexpr float kTextWaveSpeed = 0.15f; // テキストの動く速さ(フレームカウンタを参照)
	constexpr float kTextWaveSize = 4.0f; // テキストを動かす際の動く範囲
	constexpr int kStringOneSize = 1; // 1文字のサイズを示す

	constexpr int kCursorMarginX = 40; // テキストの左側にカーソルを表示するためのマージン

	constexpr unsigned int kDefaultBgColor = 0xffcabf; // 背景をBoxで表示する際の色
	constexpr unsigned int kSelectCursorColor = 0x03af7a; // カーソルの色
	constexpr unsigned int kSelectTextColor = 0x4dc4ff; // 選択されている際のテキストの色
	constexpr unsigned int kNotSelectTextColor = 0x4f4f4f; // 選択されていない際のテキストの色
}

UISelectList::UISelectList() :
	m_fontHandle(-1),
	m_bgHandle(-1),
	m_pos{},
	m_size{},
	m_targetSize{},
	m_textMoveFrameCount(0),
	m_cursor(0),
	m_isAlive(true),
	m_alignmentType(AlignmentType::Center),
	m_draw(&UISelectList::NormalDraw)
{
}

UISelectList::~UISelectList()
{
}

void UISelectList::Init()
{
}

void UISelectList::End()
{
	m_isAlive = false;
}

void UISelectList::Update()
{
	// テキストを動かす更新フレームを増加
	m_textMoveFrameCount++;

	// UIの更新フレームを増加
	m_uiUpdateFrameCount++;

	// リストが中央から出てくる際の更新処理
	if (m_uiState == UIState::AppearCenter)
	{
		// 出現割合の計算 開始時: 0.0f  終了時: 1.0f
		auto appearRate = static_cast<float>(m_uiUpdateFrameCount) / static_cast<float>(m_appearFrame);

		m_size.y = static_cast<int>(std::lerp(m_size.y, m_targetSize.y, appearRate));

		if (appearRate >= 1.0f)
		{
			m_uiState = UIState::Normal;
		}
	}
	else if (m_uiState == UIState::CloseCenter)
	{
		// 出現割合の計算 開始時: 0.0f  終了時: 1.0f
		auto appearRate = static_cast<float>(m_uiUpdateFrameCount) / static_cast<float>(m_appearFrame);

		m_size.y = static_cast<int>(std::lerp(m_size.y, 0, appearRate));

		// サイズが0以下になった場合
		if (m_size.y <= 0)
		{
			// 非アクティブに変更
			m_isActive = false;
			// 消去するかどうかを判別
			m_isAlive = !m_isAfterDelete;
		}
	}
}

void UISelectList::Draw() const
{
	(this->*m_draw)();
}

bool UISelectList::IsAlive() const
{
	return m_isAlive;
}

void UISelectList::SetSize(const Vector2Int& size)
{
	m_size = size;
	m_targetSize = size;
}

void UISelectList::SetDialog(const std::wstring title)
{
	m_dialogTitle = title;
	m_draw = &UISelectList::DialogDraw;
}

void UISelectList::SetYesNoDialog(const std::wstring title, std::function<void()> yesFunc, std::function<void()> noFunc)
{
	m_dialogTitle = title;
	m_draw = &UISelectList::DialogDraw;
	// 選択された際の処理を追加
	AddOption(L"はい", yesFunc);
	AddOption(L"いいえ", noFunc);
}

void UISelectList::AddOption(const std::wstring text, std::function<void()> onSelect)
{
	// ダイアログの状態で選択肢を2つ以上作った場合
	if (m_draw == &UISelectList::DialogDraw && m_items.size() >= kDialogItemNum)
	{
		assert(false && "ダイアログには2つ以上要素を入れられません");
		return;
	}

	// 選択オプションを追加
	m_items.push_back(OptionItem{ text, std::move(onSelect) });

	// 初回追加時にカーソルの位置を初期位置にする
	if (m_items.size() == kFirstCreateList)
	{
		m_cursor = 0;
	}
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

bool UISelectList::IsMatchedCursor(const std::wstring text)
{
	// セレクトリストの数探索
	for (int i = 0; i < m_items.size(); i++)
	{
		// 指定されたテキストがあった場合
		if (m_items[i].text == text)
		{
			// 指定されたテキストの番号と現在のカーソルが合っていたらtrueとする
			return m_cursor == i;
		}
	}

	// テキストが見つかっていない場合falseとする
	return false;
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

void UISelectList::StartAppearCenter(int appearFrame)
{
	m_uiState = UIState::AppearCenter;
	m_appearFrame = appearFrame;
	m_uiUpdateFrameCount = 0;
	m_size.y = 0;
}

void UISelectList::NormalDraw() const
{

	// テキストとカーソルをサイズの範囲内に描画する
	// テキストは左詰め
	// 上から順に均等に項目を並べる
	// 選択している項目が分かるようにするが、やり方(大きくしたり少し右寄りにするなど)は未定

	const int halfW = m_size.x / 2;
	const int halfH = m_size.y / 2;
	const int top = static_cast<int>(m_pos.y) - halfH;
	const int bottom = static_cast<int>(m_pos.y) + halfH;
	const int left = static_cast<int>(m_pos.x) - halfW;
	const int right = static_cast<int>(m_pos.x) + halfW;

	// 背景を透過する
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, kWindowAlpha);
	// 背景画像があるなら画像で、ないならBoxで背景を描画
	if (m_bgHandle != -1)
	{
		DrawExtendGraph(left, top, right, bottom, m_bgHandle, true); // ウィンドウの背景を描画
	}
	else
	{
		DrawBox(left, top, right, bottom, kDefaultBgColor, true);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 枠画像があるなら枠を表示
	//if (m_frameHandle != -1)
	//{
	//	DrawExtendGraph(left, top, right, bottom, m_frameHandle, true); // ウィンドウの背景を描画
	//}

	// ここはデフォルトから変えられるようにするかもしれない
	const int paddingX = kDefaultPaddingX;

	// テキストを描画する範囲
	const int areaW = m_size.x - paddingX * 2;

	// フォントのサイズを取得
	const int fontSize = GetFontSizeToHandle(m_fontHandle);

	// テキストを描画する範囲
	const int contentTop = top + kDefaultPaddingY;
	const int contentBottom = bottom - kDefaultPaddingY;
	const int contentHeight = contentBottom - contentTop;

	// 選択肢の数
	const int itemCount = static_cast<int>(m_items.size());
	// 選択肢同士の間隔
	const float step = static_cast<float>(contentHeight) / itemCount;

	// 項目の数だけ並べていく処理
	for (int i = 0; i < itemCount; i++)
	{
		// カーソルがこの項目にあっているか
		const bool isSelected = (i == m_cursor);
		// テキストを定義
		const std::wstring wText = m_items[i].text;
		// テキストの幅を取得
		const int lineW = GetDrawStringWidthToHandle(wText.c_str(), static_cast<int>(wText.size()), m_fontHandle);

		// 表示するX座標を確認
		int x = left + paddingX;

		// 文字の配置パターンを確認
		switch (m_alignmentType)
		{
		case AlignmentType::Left:
			// デフォルトが左詰めなので変更しない
			break;
		case AlignmentType::Right:
			// 右詰めにする
			x += right - paddingX;
			break;
		case AlignmentType::Center:
			// 文字の幅がテキストを描画する範囲内に収まっているとき
			if (lineW <= areaW)
			{
				// X座標は中央ぞろえにする
				x = left + paddingX + (areaW - lineW) / 2;
			}
			break;
		}

		// 描画する中心のY座標
		const float centerY = contentTop + step * (i + 0.5f);

		// 垂直位置の基準Y座標
		int baselineY = static_cast<int>(centerY - fontSize / 2);

		// 選択中の場合カーソルを表示する
		if (isSelected)
		{
			/*DrawTriangle(cursorCenter.x - 10, cursorCenter.y - 10,
				cursorCenter.x - 10, cursorCenter.y + 10,
				cursorCenter.x + 10, cursorCenter.y, 0xaaeef3, true);*/

			DrawStringToHandle(x - kCursorMarginX, baselineY, L"▶", kSelectCursorColor, m_fontHandle);
		}

		// 文字を1文字ずつ動かす際に進めるための値
		int advance = 0;
		// 文字が波打つスピードを調整
		float t = m_textMoveFrameCount * kTextWaveSpeed;

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
			DrawStringToHandle(x + advance, baselineY + yOffset, one.c_str(), isSelected ? kSelectTextColor : kNotSelectTextColor, m_fontHandle);

			// 文字の描画X座標をこの文字の幅分進める
			advance += cw;
		}
	}
}

void UISelectList::DialogDraw() const
{
	// テキストとカーソルをサイズの範囲内に描画する
	// テキストは左詰め
	// 上から順に均等に項目を並べる
	// 選択している項目が分かるようにするが、やり方(大きくしたり少し右寄りにするなど)は未定

	const int halfW = m_size.x / 2;
	const int halfH = m_size.y / 2;
	const int top = static_cast<int>(m_pos.y) - halfH;
	const int bottom = static_cast<int>(m_pos.y) + halfH;
	const int left = static_cast<int>(m_pos.x) - halfW;
	const int right = static_cast<int>(m_pos.x) + halfW;

	// 背景を透過する
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, kWindowAlpha);
	// 背景画像があるなら画像で、ないならBoxで背景を描画
	if (m_bgHandle != -1)
	{
		DrawExtendGraph(left, top, right, bottom, m_bgHandle, true); // ウィンドウの背景を描画
	}
	else
	{
		DrawBox(left, top, right, bottom, kDefaultBgColor, true);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 枠画像があるなら枠を表示
	//if (m_frameHandle != -1)
	//{
	//	DrawExtendGraph(left, top, right, bottom, m_frameHandle, true); // ウィンドウの背景を描画
	//}

	// ここはデフォルトから変えられるようにするかもしれない
	const int paddingX = kDefaultPaddingX;

	// テキストを描画する範囲
	const int areaW = m_size.x - paddingX * 2;

	// フォントのサイズを取得
	const int fontSize = GetFontSizeToHandle(m_fontHandle);

	// テキストを描画する範囲
	const int contentTop = top + kDefaultPaddingY;
	const int contentBottom = bottom - kDefaultPaddingY;
	const int contentLeft = left + kDefaultPaddingX;
	const int contentRight = right - kDefaultPaddingX;
	const int contentHeight = contentBottom - contentTop;
	const int contentWidth = contentRight - contentLeft;

	// 選択肢の数
	const int itemCount = static_cast<int>(m_items.size());

	// 見出しを表示
	// 見出しの幅を取得
	const int titlelineW = GetDrawStringWidthToHandle(m_dialogTitle.c_str(), static_cast<int>(m_dialogTitle.size()), m_fontHandle);
	const int titleX = (contentLeft + contentWidth / 2) - titlelineW / 2;
	const int titleY = contentTop + kDialogTitlePaddingY;
	
	// 影にするテキストを描画
	DrawStringToHandle(titleX + kOutlineMargin, titleY + kOutlineMargin, m_dialogTitle.c_str(), GetColor(0, 0, 0), m_fontHandle);
	// 実際のテキストを描画
	DrawStringToHandle(titleX, titleY, m_dialogTitle.c_str(), kSelectTextColor, m_fontHandle);

	// 左にテキストを配置したかのフラグ
	bool isSetLeft = false;

	// 項目の数だけ並べていく処理
	for (int i = 0; i < itemCount; i++)
	{
		// カーソルがこの項目にあっているか
		const bool isSelected = (i == m_cursor);
		// テキストを定義
		const std::wstring wText = m_items[i].text;
		// テキストの幅を取得
		const int lineW = GetDrawStringWidthToHandle(wText.c_str(), static_cast<int>(wText.size()), m_fontHandle);

		// 表示するY座標を確認
		int y = contentTop + contentHeight / 2;

		// 描画する中心のX座標
		float centerX = contentLeft + lineW * 0.5f + kDialogPaddingX;

		// すでに左には配置した場合
		if (isSetLeft)
		{
			// 右に配置する
			centerX = contentRight - lineW * 0.5f - kDialogPaddingX;
		}
		
		// フォントの幅を取得
		const int fontWidth = GetDrawStringWidthToHandle(wText.c_str(), static_cast<int>(wText.size()), m_fontHandle);

		// 垂直位置の基準X座標
		int baselineX = static_cast<int>(centerX - fontWidth / 2);

		// 選択中の場合カーソルを表示する
		if (isSelected)
		{
			/*DrawTriangle(cursorCenter.x - 10, cursorCenter.y - 10,
				cursorCenter.x - 10, cursorCenter.y + 10,
				cursorCenter.x + 10, cursorCenter.y, 0xaaeef3, true);*/

			DrawStringToHandle(baselineX - kCursorMarginX, y, L"▶", kSelectCursorColor, m_fontHandle);
		}

		// 文字を1文字ずつ動かす際に進めるための値
		int advance = 0;
		// 文字が波打つスピードを調整
		float t = m_textMoveFrameCount * kTextWaveSpeed;

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
			DrawStringToHandle(baselineX + advance + kOutlineMargin, y + kOutlineMargin + yOffset, one.c_str(), GetColor(0, 0, 0), m_fontHandle);
			// 実際のテキストを描画
			// 選択中の場合は色を変える
			DrawStringToHandle(baselineX + advance, y + yOffset, one.c_str(), isSelected ? kSelectTextColor : kNotSelectTextColor, m_fontHandle);

			// 文字の描画X座標をこの文字の幅分進める
			advance += cw;
		}

		// 左には配置したとする
		isSetLeft = true;
	}
}
