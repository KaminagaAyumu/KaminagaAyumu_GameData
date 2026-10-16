#pragma once
#include "../../Geometry/Vector2Int.h"
#include "../../MyLib/GameObject.h"
#include "../../MyLib/Renderer.h"
#include "../../MyLib/Component/Draw/UI/UIText.h"
#include <string>
#include <memory>

/// <summary>
/// 数値を表示するためのUI関係をまとめたクラス
/// </summary>
class ParamUI
{
public:

	ParamUI();
	virtual ~ParamUI() = default;

	void Init(const Vector2Int& pos, MyLib::Renderer::FontType type = MyLib::Renderer::FontType::Midium);

	void End();

	void SetText(const std::wstring& text);

	void SetParam(int param);

	void SetParam(const std::wstring& text);
 
	void SetActive(bool isActive);

	/// <summary>
	/// 表示位置を変更する
	/// </summary>
	/// <param name="pos">テキストとパラメータの境目の座標</param>
	void SetPos(const Vector2Int& pos);

	/// <summary>
	/// テキストとパラメータの間に表示する記号を設定する(任意)
	/// </summary>
	/// <param name="symbol"></param>
	void SetBetweenSymbol(std::wstring symbol);

private:
	std::shared_ptr<MyLib::GameObject> m_pTextObj;
	std::weak_ptr<MyLib::UIText> m_pText;

	std::shared_ptr<MyLib::GameObject> m_pParamObj;
	std::weak_ptr<MyLib::UIText> m_pParam;

	// テキストとパラメータの間に表示する記号
	// デフォルトはコロン
	std::wstring m_betweenSymbol;
};

