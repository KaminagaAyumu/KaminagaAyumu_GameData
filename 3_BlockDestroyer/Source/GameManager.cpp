#include "GameManager.h"
#include "Game.h"

namespace
{
	int kOneSecond = 60; // 1秒の単位(60フレーム毎秒時のみ)

	int kBallSpeedUpMax = 3; // ボールの速度アップの最大回数

	int kButtonGraphRadius = 14; // ボタンの画像の半径

	int kCardX = 90; // カードのX座標
	int kCardY = 30; // カードのY座標

	int kCardWidth = 140; // カードの幅
	int kCardHeight = 200; // カードの高さ

	int kCardMarginX = (Game::kGameLeftWidth / 2) - (kCardWidth / 2); // カードの間隔
	int kCardMarginY = 30; // カードの間隔

	int kGameOverY = 600; // ブロックが画面下に到達するY座標

	// 強化内容の状態を表示する場所と幅
	int kUpgradeBordX = 984;
	int kUpgradeBordY = 296;
	int kUpgradeBordWidth = 272;
	int kUpgradeBordHeight = 393;

	int kUpgradeOperatorBordStartX = 1031;
	int kUpgradeOperatorBordStartY = 369;
	int kUpgradeOperatorBordWidth = 77;
	int kUpgradeOperatorBordHeight = 52;

	int kUpgradeOperatorGaugeWidth = kUpgradeOperatorBordWidth;
	int kUpgradeOperatorGaugeHeight = 10;
	int kUpgradeOperatorGaugeMarginY = 5;

	int kProgOperateUIWidth = 66;
	int kProgOperateUIHeight = 39;

	int kScoreNumPosX = Game::kGameRightWidth + 147; // スコアの数字の開始位置X
	int kScoreNumPosY = 40; // スコアの数字の開始位置Y
	int kScoreNumWidth = 173; // スコアの数字の幅
	int kScoreNumHeight = 45; // スコアの数字の高さ

	int kFirstUpgradeNorma = 10;
	int kFirstReflectCount = 10; // 最初の反射回数による強化の条件
}

GameManager::GameManager(BallManager* ballManager, Player* player, BlockManager* blockManager)
{
	m_fontHandle = -1; // フォントハンドルを初期化
	m_fontBigHandle = -1; // 大きいフォントハンドルを初期化
	m_cardEffectHandle = -1; // カードのエフェクト画像ハンドルを初期化
	m_upgradeSeHandle = -1; // 強化抽選時のSEハンドルを初期化
	m_selectUpgradeSeHandle = -1; // 強化選択時のSEハンドルを初期化
	m_progressOperatorHandle = -1; // 進行度ゲージの画像ハンドルを初期化
	m_persentHandle = -1; // 進行度の%の画像ハンドルを初期化
	m_cardHandles = { -1, -1, -1, -1, -1, -1 };// 強化カード用の画像ハンドルを初期化
	m_upgradeUIHandle = -1; 
	m_buttonGraphHandles = { -1, -1, -1, -1 }; // ボタンの画像ハンドルを初期化
	m_upgradeOperatorHandle = { -1,-1,-1,-1,-1 }; // アップグレード内容画像ハンドルを初期化
	m_upgradeOperatorBordHandle = { -1,-1,-1,-1,-1 }; // アップグレード内容の枠画像ハンドルを初期化
	m_player = player;
	m_ballManager = ballManager;
	m_blockManager = blockManager; // ブロックマネージャーを初期化
	m_frameCount = 0; // フレーム数を初期化
	m_grAlpha = 0; // 画像の透明度を初期化
	m_fadeDirection = 0; // フェードの向きを初期化
	m_score = 0.0f; // スコアを初期化
	m_currentScore = 0.0f; // 現在のスコアを初期化
	m_prevScore = 0.0f; // 前回のスコアを初期化
	m_upgradeScoreNorma = 0; // 強化までのスコアを初期化
	m_totalBlockDestroyCount = 0; // 総ブロック破壊数を初期化
	m_prevBallReflectCount = 0; // 前回のボールの反射回数を初期化
	m_currentBallReflectCount = 0; // 現在のボールの反射回数を初期化
	m_reflectUpgradeCounter = 0; // 反射回数による強化のカウンターを初期化
	m_totalProgress = 0; // 総進行度を初期化
	m_upgradeThereshold = 0; // 強化の基準値を初期化

	m_upgrades =
	{
		UpgradeType::PlayerSizeUp,
		UpgradeType::BallSpeedUp,
		UpgradeType::BallIncrease,
		UpgradeType::BallPenetration,
		UpgradeType::BlockSpeedDown
	};

	m_selectUpgrades =
	{
		UpgradeType::PlayerSizeUp, // プレイヤーのサイズアップ
		UpgradeType::UpgradeNum, // ボールの速度アップ
		UpgradeType::UpgradeNum // ボールの増加
	};
	m_upgradeStock = 0; // 強化のストックを初期化
	m_ballSpeedUpCount = 0; // ボールの速度アップのカウントを初期化
	m_upgradeNum = 0; // アップグレードの何が行われたかを初期化
	m_isUpgradeNow = false; // 強化が行われたかどうかを初期化
	m_isShuffleUpgrades = false; // 強化抽選中の状態を初期化
	m_isResetUpgradeType = true; // 強化の種類をリセットするかどうかのフラグを初期化
	m_isPlayerUpgraded = false; // プレイヤーのアップグレード状態を初期化
	m_isBallUpgraded = false; // ボールのアップグレード状態を初期化
	m_isCanCardPressed = false; // カードが押せる状態を初期化
	m_cardBlendMode = 200; // カードのブレンドモードを初期化
}

GameManager::~GameManager()
{
}

void GameManager::Init()
{
	m_fontHandle = CreateFontToHandle("HGP教科書体", 20, -1, -1); // フォントを作成
	m_fontBigHandle = CreateFontToHandle("HGP教科書体", 40, -1, -1);
	m_cardEffectHandle = LoadGraph("data/484.png"); // カードのエフェクト画像を読み込む
	m_upgradeSeHandle = LoadSoundMem("data/upgrade_Se.mp3"); // 強化抽選時のSEを読み込む
	m_selectUpgradeSeHandle = LoadSoundMem("data/cardSelect.mp3"); // 強化選択時のSEを読み込む
	m_progressOperatorHandle = LoadGraph("data/UI_Operate%.png"); // 進行度ゲージの画像を読み込む
	m_persentHandle = LoadGraph("data/UI_%.png"); // 進行度の%の画像を読み込む
	m_cardHandles =
	{
		LoadGraph("data/card_bar.png"),
		LoadGraph("data/card_ball1.png"),
		LoadGraph("data/card_ball2.png"),
		LoadGraph("data/card_ball3.png"),
		LoadGraph("data/card_block.png"),
		LoadGraph("data/card_default1.png")
	}; // 強化カードの画像を読み込む
	m_upgradeUIHandle = LoadGraph("data/cardSelect_UI.png");
	m_buttonGraphHandles =
	{
		LoadGraph("data/A_button.png"), // Aボタンの画像を読み込む
		LoadGraph("data/B_button.png"), // Bボタンの画像を読み込む
		LoadGraph("data/X_button.png"), // Xボタンの画像を読み込む
		LoadGraph("data/Y_button.png")  // Yボタンの画像を読み込む
	};
	m_upgradeOperatorHandle =
	{
		LoadGraph("data/upgrade_bar.png"), // プレイヤー速度アップの画像を読み込む
		LoadGraph("data/upgrade_ball1.png"), // ボール速度アップの画像を読み込む
		LoadGraph("data/upgrade_ball2.png"), // ボール追加の画像を読み込む
		LoadGraph("data/upgrade_ball3.png"), // 貫通弾発動の画像を読み込む
		LoadGraph("data/upgrade_block.png") // ブロック速度ダウンの画像を読み込む
	};
	m_upgradeOperatorBordHandle = 
	{
	LoadGraph("data/upgrade_bar_bord.png"), // プレイヤー速度アップの画像を読み込む
	LoadGraph("data/upgrade_ball1_bord.png"), // ボール速度アップの画像を読み込む
	LoadGraph("data/upgrade_ball2_bord.png"), // ボール追加の画像を読み込む
	LoadGraph("data/upgrade_ball3_bord.png"), // 貫通弾発動の画像を読み込む
	LoadGraph("data/upgrade_block_bord.png") // ブロック速度ダウンの画像を読み込む
	};
	m_sound.Init(m_upgradeSeHandle); // 強化のSEを初期化
	m_ui.Init(); // UIを初期化
	m_animation.Init(m_cardEffectHandle, 12, Vector2(kCardX, kCardY), Vector2(128, 128)); // アニメーションを初期化
	m_frameCount = 0; // フレーム数を初期化
	m_grAlpha = 0; // 画像の透明度を初期化
	m_fadeDirection = Game::kOne; // フェードの向きを初期化
	m_score = 0.0f; // スコアを初期化
	m_currentScore = 0.0f; // 現在のスコアを初期化
	m_prevScore = 0.0f; // 前回のスコアを初期化
	m_upgradeScoreNorma = kFirstUpgradeNorma; // 強化までのスコアを初期化
	m_totalBlockDestroyCount = 0; // 総ブロック破壊数を初期化
	m_prevBallReflectCount = 0; // 前回のボールの反射回数を初期化
	m_currentBallReflectCount = 0; // 現在のボールの反射回数を初期化
	m_reflectUpgradeCounter = kFirstReflectCount; // 反射回数による強化のカウンターを初期化
	m_totalProgress = 0; // 総進行度を初期化
	m_upgradeThereshold = kFirstUpgradeNorma; // 強化の基準値を初期化
	m_upgrades =
	{
		UpgradeType::PlayerSizeUp,
		UpgradeType::BallSpeedUp,
		UpgradeType::BallIncrease,
		UpgradeType::BallPenetration,
		UpgradeType::BlockSpeedDown
	};

	m_selectUpgrades =
	{
		UpgradeType::UpgradeNum,
		UpgradeType::UpgradeNum,
		UpgradeType::UpgradeNum
	};
	m_upgradeStock = 0; // 強化のストックを初期化
	m_ballSpeedUpCount = 0; // ボールの速度アップのカウントを初期化
	m_upgradeNum = 0; // アップグレードの何が行われたかを初期化
	m_isUpgradeNow = false; // 強化が行われたかどうかを初期化
	m_isShuffleUpgrades = false; // 強化抽選中の状態を初期化
	m_isResetUpgradeType = true; // 強化の種類をリセットするかどうかのフラグを初期化
	m_isBallUpgraded = false; // ボールのアップグレード状態を初期化
	m_isPlayerUpgraded = false; // プレイヤーのアップグレード状態を初期化
	m_isCanCardPressed = false;
	m_cardBlendMode = 200; // カードのブレンドモードを初期化
}

void GameManager::End()
{
	// ハンドルデータを削除
	DeleteFontToHandle(m_fontHandle);
	DeleteFontToHandle(m_fontBigHandle);
	DeleteGraph(m_cardEffectHandle);
	DeleteSoundMem(m_upgradeSeHandle);
	DeleteSoundMem(m_selectUpgradeSeHandle);
	DeleteGraph(m_progressOperatorHandle);
	DeleteGraph(m_persentHandle);
	for (int i = 0; i < kAllCardNum; i++)
	{
		DeleteGraph(m_cardHandles[i]);
	}
	for (int i = 0; i < kUpGradeNum; i++)
	{
		DeleteGraph(m_upgradeOperatorHandle[i]);
		DeleteGraph(m_upgradeOperatorBordHandle[i]);
	}
	DeleteGraph(m_upgradeUIHandle);
	for (int i = 0; i < kButtonNum; i++)
	{
		DeleteGraph(m_buttonGraphHandles[i]);
	}
	m_ui.End(); // UIを終了
}

void GameManager::Update(InputManager& input)
{
	m_animation.Update(); // アニメーションを更新
	m_frameCount++; // フレーム数をカウントアップ
	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY); // マウスの位置を取得

	m_ballManager->CheckHitPlayer(*m_player); // ボールとプレイヤーの衝突をチェック

	int hitNum = m_ballManager->CheckHitBlock(*m_blockManager); // ブロックとの衝突回数を取得
	if (hitNum > 0)
	{
		for (int i = 0; i < hitNum; i++)
		{
			AddScore(); // スコアを加算
		}
		m_totalBlockDestroyCount += hitNum; // 総ブロック破壊数を更新
		m_blockManager->ResetScoreUpdate(); // スコア更新処理をリセット
	}

	/*アップグレード処理*/
	if (m_upgradeStock > 0 && m_frameCount > kOneSecond)
	{
		m_isCanCardPressed = true; // カードが押せる状態にする
		m_cardBlendMode = 0; // カードのブレンドモードを設定
		m_grAlpha += m_fadeDirection * Game::kSix;
		if (m_grAlpha >= 255)
		{
			m_grAlpha = 255;
			m_fadeDirection *= -Game::kOne;
		}
		if (m_grAlpha <= 0)
		{
			m_grAlpha = 0;
			m_fadeDirection *= -Game::kOne;
		}
	}
	else
	{
		m_isCanCardPressed = false; // カードが押せない状態にする
		m_cardBlendMode = 200;
	}

	m_currentBallReflectCount = m_ballManager->GetBallReflectionCount(); // 現在のボールの反射回数を更新

	m_totalProgress = m_ballManager->GetBallReflectionCount() + m_totalBlockDestroyCount; // 総進行度を更新
	
	// 強化の条件を満たしたとき
	if (m_totalProgress >= m_upgradeThereshold)
	{
		if (m_upgradeStock <= 0)
		{
			m_isShuffleUpgrades = true; // 強化抽選フラグを立てる
		}
		m_upgradeStock++;
		// 閾値分減算（複数回分一気に超えた場合も対応）
		while (m_totalProgress >= m_upgradeThereshold)
		{
			m_totalProgress -= m_upgradeThereshold;
			m_totalBlockDestroyCount = m_totalProgress - m_currentBallReflectCount;
		}
		m_upgradeThereshold += 5; // 次の強化の基準値を更新
		if (m_upgradeThereshold > 30)
		{
			m_upgradeThereshold = 30; // 強化の基準値の上限を設定
		}
	}

	//if (m_prevScore < m_upgradeScoreNorma && m_currentScore >= m_upgradeScoreNorma)
	//{
	//	if (m_upgradeStock <= 0)
	//	{
	//		m_isShuffleUpgrades = true; // 強化抽選フラグを立てる
	//	}
	//	m_upgradeStock++;
	//	m_upgradeScoreNorma += 10000.0f;
	//}
	//m_prevScore = m_currentScore; // 前回のスコアを更新

	// 強化が行われたとき
	if (m_isUpgradeNow)
	{
		if (m_frameCount > kOneSecond)
		{
			m_isUpgradeNow = false;
		}
	}

	// 強化抽選フラグが立った時
	if (m_isShuffleUpgrades)
	{
		if (m_isCanCardPressed)
		{
			m_isShuffleUpgrades = false; // 強化抽選フラグを下ろす
			return; // 処理を終了
		}
		ShuffleUpgrades(); // 強化の抽選を行う
	}

	// 強化の種類をリセットするフラグが立った時
	if (m_upgradeStock <= 0 && m_isResetUpgradeType)
	{
		ResetUpgradeType(); // 強化の種類をリセットする
	}

	// スコアの更新処理
	if (m_score < m_currentScore) // スコアが現在のスコアより小さい場合
	{
		float add = (m_currentScore - m_score) * 0.2f; // 現在のスコアとの差分を計算
		if (add <= 0.9f)
		{
			m_score = m_currentScore; // スコアを現在のスコアに合わせる
		}
		else
		{
			m_score += add; // スコアを更新
		}
		if (m_score > m_currentScore) // スコアが現在のスコアを超えた場合
		{
			m_score = m_currentScore; // スコアを現在のスコアに合わせる
		}
	}

	if (input.GetMouseDown(MOUSE_INPUT_LEFT)) // 左クリックが押された場合
	{
		if (!m_isCanCardPressed) // カードが押せない状態の場合
		{
			return; // 処理を終了
		}

		if (mouseX >= kCardX && mouseX <= kCardX + kCardWidth && mouseY >= kCardY && mouseY <= kCardY + kCardHeight) // 1枚目強化の領域
		{
			ApplyUpgrade(m_selectUpgrades[0]); // 強化を適用
		}
		else if (mouseX >= kCardX && mouseX <= kCardX + kCardWidth && mouseY >= kCardY + (kCardHeight + kCardMarginY) && mouseY <= kCardY + ((kCardHeight * 2) + kCardMarginY)) // 2枚目強化の領域
		{
			ApplyUpgrade(m_selectUpgrades[1]); // 強化を適用
		}
		else if (mouseX >= kCardX && mouseX <= kCardX + kCardWidth && mouseY >= kCardY + ((kCardHeight * 2) + (kCardMarginY * 2)) && mouseY <= kCardY + ((kCardHeight * 3) + (kCardMarginY * 2))) // 3枚目強化の領域
		{
			ApplyUpgrade(m_selectUpgrades[2]); // 強化を適用
		}
	}

	// Aボタン(Zキー)が押された場合
	if (input.GetPadDown(PAD_INPUT_1) || input.GetKeyDown(KEY_INPUT_Z))
	{
		if (!m_isCanCardPressed) // カードが押せない状態の場合
		{
			return; // 処理を終了
		}
		ApplyUpgrade(m_selectUpgrades[0]); // 強化を適用
	}
	// Bボタン(Xキー)が押された場合
	else if (input.GetPadDown(PAD_INPUT_2) || input.GetKeyDown(KEY_INPUT_X))
	{
		if (!m_isCanCardPressed) // カードが押せない状態の場合
		{
			return; // 処理を終了
		}
		ApplyUpgrade(m_selectUpgrades[1]); // 強化を適用
	}
	// Xボタン(Cキー)が押された場合
	else if (input.GetPadDown(PAD_INPUT_3) || input.GetKeyDown(KEY_INPUT_C))
	{
		if (!m_isCanCardPressed) // カードが押せない状態の場合
		{
			return; // 処理を終了
		}
		ApplyUpgrade(m_selectUpgrades[2]); // 強化を適用
	}


#ifdef _DEBUG
	// デバッグ用
	if (input.GetPadDown(PAD_INPUT_4))
	{
		m_animation.PlayOneShot(); // アニメーションを再生
		ShuffleUpgrades();
	}
	if (input.GetPadDown(PAD_INPUT_4) && input.GetPad(PAD_INPUT_2))
	{
		m_upgradeStock++; // 強化のストックを増やす
		printfDx("カードが押せる状態になりました。\n");
	}

#endif
}

void GameManager::Draw()
{
	for (int i = 0; i < 3; i++)
	{
		int cardNum = static_cast<int>(UpgradeType::UpgradeNum);
		switch (m_selectUpgrades[i])
		{
		case UpgradeType::PlayerSizeUp:
			cardNum = static_cast<int>(UpgradeType::PlayerSizeUp);
			break;
		case UpgradeType::BallSpeedUp:
			cardNum = static_cast<int>(UpgradeType::BallSpeedUp);
			break;
		case UpgradeType::BallIncrease:
			cardNum = static_cast<int>(UpgradeType::BallIncrease);
			break;
		case UpgradeType::BallPenetration:
			cardNum = static_cast<int>(UpgradeType::BallPenetration);
			break;
		case UpgradeType::BlockSpeedDown:
			cardNum = static_cast<int>(UpgradeType::BlockSpeedDown);
			break;
		case UpgradeType::UpgradeNum:
			cardNum = static_cast<int>(UpgradeType::UpgradeNum);
			break;
		default:
			cardNum = static_cast<int>(UpgradeType::UpgradeNum);
			break;
		}
		DrawGraph(kCardX, kCardY + (i * (kCardHeight + kCardMarginY)), m_cardHandles[cardNum], true);
	}
	
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_cardBlendMode);
	for (int i = 0; i < 3; i++)
	{
		DrawBox(kCardX, kCardY + (i * (kCardHeight + kCardMarginY)), kCardX + kCardWidth, kCardY + (i * (kCardHeight + kCardMarginY)) + kCardHeight, GetColor(0, 0, 0), true); // カードの状態の画像を描画
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	int sizeX, sizeY;
	GetGraphSize(m_upgradeUIHandle, &sizeX, &sizeY);
	for (int i = 0; i < 3; i++)
	{
		DrawGraph(kCardX - kButtonGraphRadius, kCardY - kButtonGraphRadius + (i * (kCardHeight + kCardMarginY)), m_buttonGraphHandles[i], true);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_grAlpha);
		if (m_isCanCardPressed)
		{
			DrawGraph(kCardX, kCardY - sizeY + (i * (kCardHeight + kCardMarginY)), m_upgradeUIHandle, true);
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		//DrawBox(kCardX, kCardY + (i * (kCardHeight + kCardMarginY)), kCardX + kCardWidth, kCardY + (i * (kCardHeight + kCardMarginY)) + kCardHeight, GetColor(255, 0, 0), false);
	}
	//DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(255, 0, 0), false);
	
	for (int i = 0; i < 3; i++)
	{
		m_animation.Draw(); // アニメーションを描画
		m_animation.SetPos(Vector2(kCardX + (kCardWidth / 2) - 64, kCardY + (i * (kCardHeight + kCardMarginY)) + (kCardHeight / 2) - 64)); // アニメーションの位置を設定
	}
	if (m_isUpgradeNow)
	{
		int grSizeX, grSizeY;
		GetGraphSize(m_upgradeOperatorHandle[m_upgradeNum], &grSizeX, &grSizeY);
		DrawGraph(kUpgradeBordX + (kUpgradeBordWidth / 2) - grSizeX / 2, kCardMarginY + kUpgradeBordHeight / 4, m_upgradeOperatorHandle[m_upgradeNum], true);
	}
	

	DrawGraph(kUpgradeOperatorBordStartX, kUpgradeOperatorBordStartY, m_upgradeOperatorBordHandle[2], true); // ボール増加の画像
	DrawGraph(kUpgradeOperatorBordStartX, 441, m_upgradeOperatorBordHandle[3], true); // ボール貫通の画像
	DrawGraph(1132, kUpgradeOperatorBordStartY, m_upgradeOperatorBordHandle[0], true); // プレイヤーサイズアップの画像
	DrawGraph(1132, 441, m_upgradeOperatorBordHandle[4], true); // ブロック速度ダウンの画像

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
	if (m_ballManager->GetBallNum() <= 1)
	{
		DrawBox(kUpgradeOperatorBordStartX, kUpgradeOperatorBordStartY, kUpgradeOperatorBordStartX + kUpgradeOperatorBordWidth, kUpgradeOperatorBordStartY + kUpgradeOperatorBordHeight, GetColor(0, 0, 0), true); // ボール増加の画像の上に黒い四角を描画
	}
	if (!m_player->IsExtend())
	{
		DrawBox(1132, kUpgradeOperatorBordStartY, 1132 + kUpgradeOperatorBordWidth, kUpgradeOperatorBordStartY + kUpgradeOperatorBordHeight, GetColor(0, 0, 0), true); // プレイヤーサイズアップの画像の上に黒い四角を描画
	}
	if (!m_ballManager->IsPenetration())
	{
		DrawBox(kUpgradeOperatorBordStartX, 441, kUpgradeOperatorBordStartX + kUpgradeOperatorBordWidth, 441 + kUpgradeOperatorBordHeight, GetColor(0, 0, 0), true); // ボール貫通の画像の上に黒い四角を描画
	}
	if (!m_blockManager->IsFallChange())
	{
		DrawBox(1132, 441, 1132 + kUpgradeOperatorBordWidth, 441 + kUpgradeOperatorBordHeight, GetColor(0, 0, 0), true); // ブロック速度ダウンの画像の上に黒い四角を描画
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	if (m_player->IsExtend())
	{
		m_ui.DrawGauge(1132, kUpgradeOperatorBordStartY + kUpgradeOperatorBordHeight + kUpgradeOperatorGaugeMarginY, kUpgradeOperatorGaugeWidth, kUpgradeOperatorGaugeHeight, m_player->GetExtendTimeRate());
	}
	if (m_ballManager->IsPenetration())
	{
		m_ui.DrawGauge(kUpgradeOperatorBordStartX, 441 + kUpgradeOperatorBordHeight + kUpgradeOperatorGaugeMarginY, kUpgradeOperatorGaugeWidth, kUpgradeOperatorGaugeHeight, m_ballManager->GetPenetrationTimeRate());
	}
	if (m_blockManager->IsFallChange())
	{
		m_ui.DrawGauge(1132, 441 + kUpgradeOperatorBordHeight + kUpgradeOperatorGaugeMarginY, kUpgradeOperatorGaugeWidth, kUpgradeOperatorGaugeHeight, m_blockManager->GetFallChangeTimeRate());
	}

	m_ui.DrawNumber((int)m_score, kScoreNumPosX, kScoreNumPosY, 0.4f); // スコアを描画)
	//m_ui.DrawNumberInSize((int)m_score, kScoreNumPosX, kScoreNumPosY, kScoreNumWidth, kScoreNumHeight);

	// 進捗率（0.0～1.0）
	float progressRate = (float)(m_currentBallReflectCount + m_totalBlockDestroyCount) / m_upgradeThereshold;
	if (progressRate > 1.0f)
	{
		progressRate = 1.0f; // 進捗率が1.0を超えた場合は1.0に設定
	}
	
	m_ui.DrawGauge(kUpgradeOperatorBordStartX, kUpgradeOperatorBordStartY + 180, 1132 + kUpgradeOperatorBordWidth - kUpgradeOperatorBordStartX, 20, progressRate);

	DrawGraph(kUpgradeOperatorBordStartX, kUpgradeOperatorBordStartY + 215, m_progressOperatorHandle, true); // 進行度ゲージの画像を描画
	int persent = static_cast<int>(progressRate * 100.0f); // 進捗率をパーセントに変換
	m_ui.DrawNumber(100 - persent, kUpgradeOperatorBordStartX + kProgOperateUIWidth, kUpgradeOperatorBordStartY + 215, 76 / 3, kProgOperateUIHeight); // 現在の進行度を描画
	//m_ui.DrawNumberInSizeCenter(100 - persent, kUpgradeOperatorBordStartX + kProgOperateUIWidth, kUpgradeOperatorBordStartY + 215, 76, kProgOperateUIHeight);
	DrawGraph(1132 + kUpgradeOperatorBordWidth - 36, kUpgradeOperatorBordStartY + 215, m_persentHandle, true); // %の画像を描画

}

void GameManager::AddScore()
{
	m_currentScore += 1000; // スコアを1000加算
}

void GameManager::PlayerSizeUp()
{
	m_player->ChangeGraphExtRate(1.5f, 1.0f); // プレイヤーの拡大率を変更
}

void GameManager::BallSpeedUp()
{
	if (m_ballSpeedUpCount >= kBallSpeedUpMax) // ボールの速度アップの最大回数を超えた場合
	{
		//printfDx("ボールの速度アップは最大回数に達しました。\n"); // メッセージを表示
		return; // 処理を終了
	}
	int ballNum = m_ballManager->GetBallArraySize(); // ボールの数を取得
	for (int i = 0; i < ballNum; i++)
	{
		Ball& ball = m_ballManager->GetBall(i); // ボールを取得
		if (!ball.IsExist())
		{
			continue; // ボールが存在しない場合はスキップ
		}
		ball.ChangeMaxSpeed(0.5f); // ボールの最大速度を1.0増やす
	}
	m_ballSpeedUpCount++; // ボールの速度アップ回数を増やす
}

void GameManager::BallIncrease()
{
	int num = m_ballManager->GetBallNum(); // 現在のボールの数を取得
	if (num > 3)
	{
		//printfDx("これ以上増やせません。\n");
		return;
	}
	Ball newBall;
	m_ballManager->AddBall(newBall, m_player->GetPos()); // 新しいボールを追加
}

void GameManager::BallPenetration()
{
	if (m_isBallUpgraded)
	{
		//printfDx("ボールは既に貫通強化されています。\n"); // 既にアップグレードされている場合のメッセージ
		return; // 処理を終了
	}

	m_ballManager->BallPenetation(); // ボールの貫通能力を有効にする

	m_isBallUpgraded = true; // アップグレード状態を更新
}

void GameManager::BlockSpeedDown()
{
	m_blockManager->ChangeFallTime(300.0f);
}

bool GameManager::IsGameOver() const
{
	int blockNum = m_blockManager->GetBlockArraySize(); // ブロックの数を取得
	for (int i = 0; i < blockNum; i++)
	{
		const Block& block = m_blockManager->GetBlock(i); // ブロックを取得
		if (!block.IsExist)
		{
			continue; // ブロックが存在しない場合はスキップ
		}
		if (block.Pos.Y >= kGameOverY)
		{
			return true; // ブロックが画面下に到達した場合、ゲームオーバー
		}
	}
	int ballNum = m_ballManager->GetBallNum(); // ボールの残りの数を取得
	if (ballNum <= 0)
	{
		return true; // ボールが全て存在しない場合、ゲームオーバー
	}
	return false;
}

bool GameManager::IsGameClear(bool isClear)
{
	if (isClear)
	{
		return true; // ブロックが全て消えた場合、ゲームクリア
	}
	return false;
}

bool GameManager::IsGameClear() const
{
	if (m_blockManager->GetBlockNum() <= 0)
	{
		return true; // ブロックが全て消えた場合、ゲームクリア
	}
	return false;
}

void GameManager::ShuffleUpgrades()
{
	m_sound.Play(); // 強化抽選のSEを再生
	m_animation.PlayOneShot(); // アニメーションを再生
	// 強化抽選
	for (int i = 0; i < static_cast<int>(m_selectUpgrades.size()); i++)
	{
		int num = GetRand(static_cast<int>(UpgradeType::BlockSpeedDown)); // 0から4のランダムな整数を取得
		if (m_ballSpeedUpCount >= 3)
		{
			if (m_upgrades[num] == UpgradeType::BallSpeedUp) // ボール速度アップが選ばれた場合
			{
				//printfDx("速度アップは選べない\n");
				m_upgrades[num] = UpgradeType::BlockSpeedDown; // ボール速度アップ以外の強化を選ぶ
			}
		}
		if(m_ballManager->GetBallNum() > 3)
		{
			if (m_upgrades[num] == UpgradeType::BallIncrease) // ボール増加が選ばれた場合
			{
				//printfDx("ボール増加は選べない\n");
				m_upgrades[num] = UpgradeType::BlockSpeedDown; // ボール増加以外の強化を選ぶ
			}
		}
		if (m_isBallUpgraded)
		{
			if (m_upgrades[num] == UpgradeType::BallPenetration) // ボール貫通が選ばれた場合
			{
				//printfDx("ボール貫通は選べない\n");
				m_upgrades[num] = UpgradeType::BlockSpeedDown; // ボール貫通以外の強化を選ぶ
			}
		}
		m_selectUpgrades[i] = m_upgrades[num]; // 選択された強化を設定

	}
	if (m_selectUpgrades[0] == m_selectUpgrades[1] || m_selectUpgrades[0] == m_selectUpgrades[2] || m_selectUpgrades[1] == m_selectUpgrades[2])
	{
		ShuffleUpgrades();
		return;
	}
	m_frameCount = 0; // フレーム数をリセット
	m_isShuffleUpgrades = false; // 強化抽選中の状態をリセット
}

void GameManager::ApplyUpgrade(UpgradeType upgrade)
{
	m_isUpgradeNow = true; // 強化が行われた
	m_frameCount = 0; // フレーム数をリセット
	m_upgradeStock--; // 強化のストックを減らす
	if (m_upgradeStock > 0)
	{
		ShuffleUpgrades();
	}
	else
	{
		m_isResetUpgradeType = true; // 強化の種類をリセットするフラグを立てる
	}
	m_sound.Play(m_selectUpgradeSeHandle); // 強化選択のSEを再生

	switch (upgrade)
	{
	case UpgradeType::PlayerSizeUp:
		//printfDx("プレイヤーサイズアップ\n");
		m_upgradeNum = static_cast<int>(UpgradeType::PlayerSizeUp);
		PlayerSizeUp();
		break;
	case UpgradeType::BallSpeedUp:
		//printfDx("ボール速度アップ\n");
		m_upgradeNum = static_cast<int>(UpgradeType::BallSpeedUp);
		BallSpeedUp();
		break;
	case UpgradeType::BallIncrease:
		//printfDx("ボール増加\n");
		m_upgradeNum = static_cast<int>(UpgradeType::BallIncrease);
		BallIncrease();
		break;
	case UpgradeType::BallPenetration:
		//printfDx("ボール貫通\n");
		m_upgradeNum = static_cast<int>(UpgradeType::BallPenetration);
		BallPenetration();
		break;
	case UpgradeType::BlockSpeedDown:
		//printfDx("ブロック速度ダウン\n");
		m_upgradeNum = static_cast<int>(UpgradeType::BlockSpeedDown);
		BlockSpeedDown();
		break;
	case UpgradeType::UpgradeNum:
		//printfDx("アップグレードの種類が無効です。\n");
		m_upgradeNum = 0;
		break;
	default:
		break;
	}

}

void GameManager::ResetUpgradeType()
{
	for (int i = 0; i < static_cast<int>(m_selectUpgrades.size()); i++)
	{
		m_selectUpgrades[i] = UpgradeType::UpgradeNum; // 選択された強化をリセット
	}
	//printfDx("アップグレードの種類をリセットしました。\n");
	m_isResetUpgradeType = false; // フラグをリセット
}
