#pragma once
#include "SceneBase.h"
#include <memory>
#include <vector>
#include <map>
#include <string>

class UIText;
class UISelectList;
class Model;

class ResultScene : public SceneBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="controller">シーン管理用クラス(継承元で取得しなければならない)</param>
	ResultScene(SceneController& controller);
	virtual ~ResultScene();

	/// <summary>
	/// シーンの更新処理
	/// </summary>
	/// <param name="input">入力情報を取得するクラス</param>
	void Update(Input& input) override;
	/// <summary>
	/// シーンの描画処理
	/// </summary>
	void Draw() override;

private:

	// 更新処理用の関数ポインタの別名を定義
	using UpdateFunc_t = void(ResultScene::*)(Input&);
	UpdateFunc_t m_update; // 更新処理を定義

	// 描画処理用の関数ポインタの別名を定義
	using DrawFunc_t = void(ResultScene::*)();
	DrawFunc_t m_draw; // 描画処理を定義

	// 描画用のスコア
	int m_resultScore;

	// タイムボーナスのスコア
	int m_timeBonusScore;

	// 描画用の時間
	int m_resultTime;

	// テキストを表示してから待つ時間
	int m_waitFrame;

	// スコアボーナスを取得することができる最大時間
	float m_scoreBonusMaxTime;

	// 演出用のプレイヤーのモデルのハンドル
	int m_playerModelHandle;

	// クリアタイムを更新し終わったかどうか
	bool m_isTimeResultEnd;

	// プレイヤーのリアクションのアニメーションが終わったかどうか
	bool m_isPlayerReactionEnd;

	// ボーナススコアを表示するテキスト
	std::weak_ptr<UIText> m_pBonusScoreText;
	// スコアを表示するテキスト
	std::weak_ptr<UIText> m_pScoreText;
	// 時間を表示するテキスト
	std::weak_ptr<UIText> m_pTimeText;
	// シーン遷移の選択肢を表示するリスト
	std::weak_ptr<UISelectList> m_pSelectList;

	// 演出として使用するプレイヤーのモデル
	std::shared_ptr<Model> m_pPlayerModel;

	// リザルトのテキスト群を取得
	std::map<std::string, std::vector<std::weak_ptr<UIText>>> m_resultTexts;

	// メンバ関数
private:
	// 更新処理
	void FadeInUpdate(Input&);
	void NormalUpdate(Input&);
	void FadeOutUpdate(Input&);
	void ClearTimeUpdate(Input&);
	void ScoreUpdate(Input&);
	void BonusScoreUpdate(Input&);
	void ScoreAddUpdate(Input&);

	// 描画処理
	void NormalDraw();
	void FadeDraw();

	// ボーナススコアをチェックする
	void CheckBonusScore();

	/// <summary>
	/// テキストをアクティブにする
	/// </summary>
	/// <param name="activeText"></param>
	void ActiveText(std::string activeText);

};
