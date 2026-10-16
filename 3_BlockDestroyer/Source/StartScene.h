#pragma once
#include "SceneBase.h"
#include "Game.h"
#include "DxLib.h"

class StartScene : public SceneBase
{
public:
	// コンストラクタ
	StartScene();
	// デストラクタ
	virtual ~StartScene();
	// メンバ関数
	virtual void Init() override;	// 初期化
	virtual void End() override;		// 終了処理
	virtual void Update() override;	// 毎フレ－ムの更新処理	
	virtual void Draw() override;	// Update()での更新を踏まえて描画を行う

	bool IsSceneEnd() { return m_isEnd; } // シーン終了フラグを設定する
private:
	int m_castleBgHandle; // 城の背景画像ハンドル
	int m_startHandle; // スタート用ハンドル

	int m_StartAlphaCount;
	int m_startFadeAlpha;

};

