#include "TextResource.h"
#include <cassert>

namespace
{
	// CSVの列番号
	constexpr int kIdColumn = 0;	// テキストのID
	constexpr int kTextColumn = 1;	// 表示するテキスト
}

void TextResource::ConvertTextData()
{
	// データの数を取得する
	size_t size = GetDataCount();

	for (size_t i = 0; i < size; ++i)
	{
		// データを読み込む
		std::wstring id = Read<std::wstring>(static_cast<int>(i), kIdColumn);
		std::wstring text = Read<std::wstring>(static_cast<int>(i), kTextColumn);

		// 攻撃データを作成
		std::shared_ptr<TextData> data = std::make_shared<TextData>();
		// データをセットする
		data->SetData(id, text);
		m_pTextDatas[id] = data;
	}

	// データを開放する
	ReleaseRawData();
}

std::wstring TextResource::GetText(std::wstring id)
{
	auto it = m_pTextDatas.find(id);
	if (it != m_pTextDatas.end())
	{
		return it->second.get()->GetText();
	}
	else
	{
		assert(false && "TextResource : IDに対応するテキストのデータが見つかりませんでした");
	}
	return std::wstring();
}
