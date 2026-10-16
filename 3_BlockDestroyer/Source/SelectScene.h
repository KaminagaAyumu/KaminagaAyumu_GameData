#pragma once
#include "DxLib.h"
#include "Vector2.h"
#include "SceneBase.h"
class SelectScene : public SceneBase
{
public:
	// コンストラクタ
	SelectScene();
	// デストラクタ
	virtual ~SelectScene();
	// メンバ関数
	virtual void Init() override;	// 初期化
	virtual void End() override;		// 終了処理
	virtual void Update() override;	// 毎フレ－ムの更新処理	
	virtual void Draw() override;	// Update()での更新を踏まえて描画を行う

	// 終了フラグの取得
	bool IsSceneEnd() { return m_isEnd; }
private:

	int m_selectTutorialHandle;
	int m_selectStartHandle;
	int m_tutorialGraphHandle;
	bool m_isTutorial;

	Vector2 m_cursorPos;	// カーソルの位置
};

