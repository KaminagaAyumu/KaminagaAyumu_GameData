#pragma once
#include <sstream>
#include <unordered_map>

/// <summary>
/// テキストのデータを管理する構造体
/// </summary>
struct TextData
{
	std::string id;
	int page;
	std::string textData;
};

class TextManager
{
public:
	TextManager();
	virtual ~TextManager();

	void Init();
	void Update();
	void Draw();

	void LoadTextData();

	/// <summary>
	/// 取得したIDのテキストデータの1ページ目を返す
	/// </summary>
	/// <param name="id">テキストID</param>
	/// <returns>テキストデータ</returns>
	std::string GetFirstPageText(const std::string& id);

	/// <summary>
	/// 取得したIDのテキストデータのすべてのページを返す
	/// </summary>
	/// <param name="id">テキストID</param>
	/// <returns>テキストデータ</returns>
	std::vector<TextData> GetAllPageText(const std::string& id);

private:
	// テキストのデータを格納する配列
	std::unordered_map<std::string, std::vector<TextData>> m_textData;


};

