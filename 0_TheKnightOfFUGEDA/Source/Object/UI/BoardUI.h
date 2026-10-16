#pragma once
#include "../../Geometry/Vector2Int.h"
#include "../../MyLib/Renderer.h"
#include "../../MyLib/Component/Draw/UI/UIText.h"
#include "../../Utility/File/File.h"
#include <string>
#include <memory>
#include <vector>

class ParamUI;

/// <summary>
/// 見出しと、テキストと値の組(ParamUI)を上から並べて表示するボード
/// ハイスコアの表示やチュートリアルの操作説明などに使う
/// </summary>
class BoardUI
{
public:

	BoardUI();
	virtual ~BoardUI();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="pos">ボード全体の中心座標</param>
	/// <param name="size">ボード全体のサイズ</param>
	/// <param name="type">見出しと各行のフォントの種類</param>
	void Init(const Vector2Int& pos, const Vector2Int& size, MyLib::Renderer::FontType type = MyLib::Renderer::FontType::Small);

	/// <summary>
	/// 終了処理(ボードが持つすべてのUIを終了させる)
	/// </summary>
	void End();

	/// <summary>
	/// 背景の画像を設定する(任意)
	/// 画像はボード全体のサイズに拡縮して表示する
	/// 描画順の都合上、SetTitleやAddParamより前に呼ぶこと
	/// </summary>
	/// <param name="pFile">背景にする画像ファイル</param>
	void SetBackGround(std::shared_ptr<File> pFile);

	/// <summary>
	/// 見出しのテキストを設定する
	/// </summary>
	/// <param name="title">見出しのテキスト</param>
	void SetTitle(const std::wstring& title);

	/// <summary>
	/// 行を一番下に追加する
	/// </summary>
	/// <param name="text">行の左側に表示するテキスト</param>
	/// <param name="param">行の右側に表示する値</param>
	void AddParam(const std::wstring& text, int param);

	/// <summary>
	/// 行を一番下に追加する
	/// </summary>
	/// <param name="text">行の左側に表示するテキスト</param>
	/// <param name="param">行の右側に表示する文字列</param>
	void AddParam(const std::wstring& text, const std::wstring& param);

	/// <summary>
	/// 追加済みの行の値を変更する
	/// </summary>
	/// <param name="index">行の番号(上から0始まり)</param>
	/// <param name="param">表示する値</param>
	void SetParam(int index, int param);

	/// <summary>
	/// ボード全体の表示状態を設定する
	/// </summary>
	/// <param name="isActive">true : 表示する false : 表示しない</param>
	void SetActive(bool isActive);

	/// <summary>
	/// 各行のテキストと値の境目の位置を調整する(任意)
	/// 境目の初期位置はボードの左右中央で、テキストは境目の左側、値は右側に表示される
	/// テキストが長くてボードからはみ出る場合は、正の値を設定して境目を右にずらす
	/// 追加済みの行にも反映される
	/// </summary>
	/// <param name="offsetX">ボードの左右中央からのX方向のずらし量(正の値で右、負の値で左)</param>
	void SetSeparatorOffsetX(int offsetX);

private:

	/// <summary>
	/// 指定した行の中心座標を取得する
	/// </summary>
	/// <param name="index">行の番号(上から0始まり)</param>
	/// <returns>行の中心座標</returns>
	Vector2Int GetRowPos(int index) const;

	// ボード全体の中心座標
	Vector2Int m_pos;
	// ボード全体のサイズ
	Vector2Int m_size;
	// 見出しと各行のフォントの種類
	MyLib::Renderer::FontType m_fontType;
	// 各行のテキストと値の境目の、ボードの左右中央からのずらし量
	int m_separatorOffsetX;

	// 背景の画像のオブジェクト
	std::shared_ptr<MyLib::GameObject> m_pBgObj;
	// 見出しのテキストのオブジェクト
	std::shared_ptr<MyLib::GameObject> m_pTitleObj;

	std::weak_ptr<MyLib::UIText> m_pTitle;
	// 各行のUI(上から順に並ぶ)
	std::vector<std::shared_ptr<ParamUI>> m_pParamUIs;

	// ボード全体の表示状態(後から追加した行にも反映する)
	bool m_isActive;
};

