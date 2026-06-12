#pragma once
#include "SceneBase.h"
#include "../Common/Game.h"
#include "../Geometry/Vector3.h"
#include <memory>
#include <vector>
#include <map>
#include <string>

class UIText;
class UIImage;
class UISelectList;
class Model;

class ClearScene : public SceneBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="controller">シーン管理用クラス(継承元で取得しなければならない)</param>
	ClearScene(SceneController& controller);
	virtual ~ClearScene();

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
	using UpdateFunc_t = void(ClearScene::*)(Input&);
	UpdateFunc_t m_update; // 更新処理を定義

	// 描画処理用の関数ポインタの別名を定義
	using DrawFunc_t = void(ClearScene::*)();
	DrawFunc_t m_draw; // 描画処理を定義

	// 描画用のスコア
	int m_resultScore;

	// タイムボーナスのスコア
	int m_timeBonusScore;

	// 描画用の時間
	int m_resultTime;

	// テキストを表示してから待つ時間
	int m_waitFrame;

	// プレイヤーのモデルのハンドル
	int m_playerModelHandle;
	// 敵のモデルのハンドル
	int m_enemyModelHandle;

	// スコアボーナスを取得することができる最大時間
	float m_scoreBonusMaxTime;

	// クリアタイムを更新し終わったかどうか
	bool m_isTimeResultEnd;

	// ボーナススコアを表示するテキスト
	std::weak_ptr<UIText> m_pBonusScoreText;

	// スコアを表示するテキスト
	std::weak_ptr<UIText> m_pScoreText;

	// 時間を表示するテキスト
	std::weak_ptr<UIText> m_pTimeText;

	// セレクトリストを取得するポインタ
	std::weak_ptr<UISelectList> m_pSelectList;

	// ベストスコアとベストタイムを表示する際のテキストボックス画像
	std::weak_ptr<UIImage> m_pTextBoxImage;

	// リザルトのテキスト群を取得
	std::map<std::string, std::vector<std::weak_ptr<UIText>>> m_resultTexts;

	// ベストスコアとベストタイムというテキストを表示するためのvector
	std::vector<std::weak_ptr<UIText>> m_pRecordTitleTexts;

	// ベストスコアとベストタイムを表示するテキスト群を取得
	std::map<StageType, std::vector<std::weak_ptr<UIText>>> m_recordTexts;

	// プレイヤーのモデル
	std::shared_ptr<Model> m_pPlayerModel;
	// 敵のモデル
	std::shared_ptr<Model> m_pEnemyModel;

	// 敵とプレイヤーのモデルの表示位置
	Vector3 m_playerModelPos;
	Vector3 m_enemyModelPos;

	// 敵とプレイヤーのモデルが動く向き
	Vector3 m_moveDir;

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
	void RecordUpdate(Input&);

	// 描画処理
	void NormalDraw();
	void FadeDraw();

	// ボーナススコアをチェックする
	void CheckBonusScore();

	/// <summary>
	/// ベストスコア、ベストタイムを取得する
	/// </summary>
	void SetRecord();

	/// <summary>
	/// レコードのテキストの表示状態を変更する
	/// </summary>
	/// <param name="isActive">true : 表示する false : 表示しない</param>
	void SetRecordActive(bool isActive);

	/// <summary>
	/// テキストをアクティブにする
	/// </summary>
	/// <param name="activeText"></param>
	void ActiveText(std::string activeText);

	/// <summary>
	/// モデルを更新する
	/// </summary>
	void UpdateModel();

};