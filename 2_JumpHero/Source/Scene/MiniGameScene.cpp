#include "MiniGameScene.h"
#include "SceneController.h"
#include "ClearScene.h"
#include "PauseScene.h"
#include "GameoverScene.h"
#include "../Game/GameManager.h"
#include "../Game/CollisionManager.h"
#include "../Game/Actor.h"
#include "../Game/Effect/EffectManager.h"
#include "../Utility/UI/UIManager.h"
#include "../Utility/UI/UIFormatText.h"
#include "../Utility/UI/UIGauge.h"
#include "../Utility/Application.h"
#include "../Utility/Sound/SoundManager.h"
#include "../Game/TextManager.h"
#include "../Game/Event/EventManager.h"
#include "../Utility/Bg.h"
#include "../Utility/Map.h"
#include "../Utility/Camera.h"
#include "../Utility/Game.h"
#include "../Utility/Input.h"
#include "DxLib.h"
#include <cassert>

namespace
{
	constexpr int kFadeInterval = 60; // フェード処理を行う時間

	constexpr int kDescriptionInterval = 10; // 説明の表示を行う時間
	constexpr int kDescriptionBorderMargin = 50; // 説明表示の上下の余白

	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値

	constexpr int kHeaderMargin = 60; // ヘッダー(見出し)の余白

	constexpr float kTemporaryBGMFadeTime = 30.0f; // スタックとして積んだBGMのフェード時間

	constexpr float kGoalBGMVolume = 20.0f; // ゴール時のBGMのボリューム

	constexpr float kScoreTextPosY = 20.0f;

	constexpr int kBalloonGaugeNo = 0; // 風船のゲージの番号
	const Size kBalloonGaugeSize = { 200, 40 }; // UIで使用するゲージのサイズ
	const Position2 kBalloonGaugePos = { 20.0f,60.0f }; // 風船のゲージの座標

	const Size kCoinIconSize = { 40,40 };
	const Position2 kCoinIconPos = { 20.0f, 60.0f };

	const Size kPlayerIconSize = { 30,30 };
	const Position2 kPlayerIconPos = { 20.0f, 110.0f };
	const Position2 kLifeTextPos = { 45.0f, 110.0f };

	const Size kMedalIconSize = { 32,32 };
	const Position2 kMedalIconPos = { 20.0f,145.0f };
	const Position2 kMedalTextPos = { 45.0f,145.0f };

	const Position2 kSpawnFlotingItemPos{ 200.0f,200.0f };
}

MiniGameScene::MiniGameScene(SceneController& controller, std::shared_ptr<GameManager> gameManager, int stageNo) :
	SceneBase(controller),
	m_fadeColor(0xffffff),
	m_isInputOK(false),
	m_updateFunc(&MiniGameScene::FadeInUpdate),
	m_drawFunc(&MiniGameScene::FadeDraw),
	m_pGameManager(gameManager)
{
	m_frameCount = kFadeInterval;

	m_stageNo = stageNo;

	m_bg = std::make_shared<Bg>();
	m_bg->SetBgType(Types::BgType::ScrollXY);
	m_bg->Init();


	m_pMap = std::make_shared<Map>(stageNo,true);
	m_pMap->Init();

	m_pCamera = std::make_shared<Camera>(m_pMap->GetMapSize());

	m_pUIManager = std::make_shared<UIManager>();

	m_pScoreText = m_pUIManager->CreateFormatText(Types::FontType::Small, "", { 0, kScoreTextPosY });
	auto score = m_pScoreText.lock();
	score->SetProvider([this]()
		{
			return std::string("スコア:") + std::to_string(m_pGameManager->GetScore());
		});

	m_pLifeText = m_pUIManager->CreateFormatText(Types::FontType::Small, "", kLifeTextPos);
	auto life = m_pLifeText.lock();
	life->SetProvider([this]()
		{
			return std::string("×") + std::to_string(m_pGameManager->GetLife());
		});

	m_pMedalText = m_pUIManager->CreateFormatText(Types::FontType::Small, "", kMedalTextPos);
	auto medal = m_pMedalText.lock();
	medal->SetProvider([this]()
		{
			return std::string("×") + std::to_string(m_pGameManager->GetMedalNum());
		});

	m_pDropItemGauge = m_pUIManager->CreateGauge(kBalloonGaugeSize, kBalloonGaugePos, kBalloonGaugeNo);

	m_pUIManager->CreateImage(Types::ImageType::ChangeCoinIcon, kCoinIconSize, kCoinIconPos);
	m_pUIManager->CreateImage(Types::ImageType::PlayerIcon, kPlayerIconSize, kPlayerIconPos);
	m_pUIManager->CreateImage(Types::ImageType::MedalIcon, kMedalIconSize, kMedalIconPos);

	m_pTextManager = std::make_unique<TextManager>();

	m_pEffectManager = std::make_shared<EffectManager>(controller.GetEffekseerResourceManager());
	m_pEffectManager->SetCamera(m_pCamera);

	m_pGameManager->MiniGameInit(m_pMap,m_pCamera,m_pEffectManager,m_pUIManager, m_pActors);

	m_pCamera->SetTargetProvider([this]() {return m_pGameManager->GetPlayerPos(); });
	m_pCamera->Init();

	m_pCollisionManager = std::make_unique<CollisionManager>();

	// イベントセンサーのポインタを取得
	m_pEventSensors = std::make_shared<EventSensors>();
	m_pEventControls = std::make_shared<EventControls>();

	Application::GetInstance().GetSoundManager()->LoadSoundClip("pauseSE", L"data/sound/SE/pauseSE.mp3", SoundBus::SE, 1.0f, false);
	Application::GetInstance().GetSoundManager()->LoadSoundClip("goalSE", L"data/sound/SE/goalSE.mp3", SoundBus::SE, kGoalBGMVolume, false);

	// イベントの内容を格納
	SetEventFunc();

	m_pEventManager = std::make_unique<EventManager>();
	if (!m_pEventManager->LoadCommonEventData(stageNo, true) || !m_pEventManager->LoadEventData(stageNo, true))
	{
		assert(false && "イベントデータの読み込みに失敗しました");
	}

	// イベントのデータをイベントマネージャーに渡す
	m_pEventManager->SetEvents(m_pEventControls, m_pEventSensors);

}

MiniGameScene::~MiniGameScene()
{
}

void MiniGameScene::Update(Input& input)
{
	(this->*m_updateFunc)(input);
}

void MiniGameScene::Draw()
{
	(this->*m_drawFunc)();
}

void MiniGameScene::FadeInUpdate(Input&)
{
	m_frameCount--;
	if (m_frameCount <= 0)
	{
		// フェードイン完了
		m_updateFunc = &MiniGameScene::NormalUpdate;
		m_drawFunc = &MiniGameScene::NormalDraw;
		return; // 念のため処理を抜ける
	}
}

void MiniGameScene::NormalUpdate(Input& input)
{
	// ポーズボタンが押されたら
	if (input.IsTriggered("Pause"))
	{
		Application::GetInstance().GetSoundManager()->Play("pauseSE", 1.0f, true);
		// ポーズシーンをプッシュする(このシーンに戻ることも可能にする)
		m_controller.PushScene(std::make_shared<PauseScene>(m_controller));
		return;
	}

	if (auto gauge = m_pDropItemGauge.lock())
	{
		gauge->SetValue(m_pGameManager->GetMiniGameBalloonCounterRate());
	}

	// OKボタンが押されたかどうかを判定する
	m_isInputOK = input.IsTriggered("OK");

	m_pUIManager->Update();

	// イベントの更新
	m_pEventManager->Update();

	// ゲームを止めるフラグが立っていたら
	if (m_pEventManager->IsFreezeGame())
	{
		// オブジェクトの更新を止める
		return;
	}

	// カメラの更新
	m_pCamera->Update();

	// エフェクトマネージャーを更新
	m_pEffectManager->Update();

	// ゲームマネージャーの更新
	m_pGameManager->Update(input);

	// 次のフレームのためにゲーム内オブジェクトを更新
	m_pActors.clear(); // オブジェクトをリセット
	m_pActors.reserve(m_pGameManager->GetActorNum()); // 現在のオブジェクトの総数分要素を確保 
	m_pGameManager->PushActors(m_pActors); // ゲームマネージャーからオブジェクトを受け取る

	// 当たり判定を行う
	if (!m_pGameManager->IsSkipCollision()) // 当たり判定をスキップしない場合
	{
		m_pCollisionManager->CheckCollision(m_pActors);
	}

	// 背景とマップの更新(現在何もやっていない)
	m_bg->Update();
	m_pMap->Update();

	if (m_pGameManager->IsClear()) // ゲームマネージャーがクリアと判定したら
	{
		Application::GetInstance().GetSoundManager()->Play("goalSE", 1.0f, true);
		// BGMが変更されている場合があるのでそれをリセットする
		Application::GetInstance().GetSoundManager()->EndTemporaryBGM(kTemporaryBGMFadeTime);
		Application::GetInstance().GetSoundManager()->ClearBGMStack();
		// シーンの終了へ
		m_fadeColor = 0xffffff; // 白でフェードアウト
		m_updateFunc = &MiniGameScene::FadeOutUpdate;
		m_drawFunc = &MiniGameScene::FadeDraw;
		return; // 念のため処理を抜ける
	}

	if (m_pGameManager->IsGameOver()) // ゲームオーバーと判定したら
	{
		// BGMが変更されている場合があるのでそれをリセットする
		Application::GetInstance().GetSoundManager()->EndTemporaryBGM(kTemporaryBGMFadeTime);
		Application::GetInstance().GetSoundManager()->ClearBGMStack();
		// このシーンの終了処理
		m_fadeColor = 0xffffff; // フェードを白フェードにする
		m_updateFunc = &MiniGameScene::FadeOutUpdate;
		m_drawFunc = &MiniGameScene::FadeDraw;
		return;
	}
}

void MiniGameScene::FadeOutUpdate(Input&)
{
	m_frameCount++;
	if (m_frameCount >= kFadeInterval)
	{
		// フェードアウト完了
		// ゲームオーバーだったら
		if (m_pGameManager->IsGameOver())
		{
			m_controller.ChangeScene(std::make_shared<GameoverScene>(m_controller, m_pGameManager, m_stageNo));
		}
		else // ゲームクリアだったらここに入る(今のところ条件がそれ以外ないのでゲームオーバーでない場合とする)
		{
			m_controller.ChangeScene(std::make_shared<ClearScene>(m_controller, m_pGameManager, m_stageNo));
		}
		return; // 念のため処理を抜ける
	}
}

void MiniGameScene::NormalDraw()
{
	m_bg->Draw(m_pCamera);
	m_pMap->Draw(m_pCamera);

	for (auto& actor : m_pActors)
	{
		// Actorの描画関数を呼ぶためにlockする
		if (auto pActor = actor.lock())
		{
			pActor->Draw();
		}
	}

	m_pUIManager->Draw();

	m_pGameManager->Draw();

	m_pEffectManager->Draw();

#ifdef _DEBUG
	DrawString(0, 0, L"MiniGameScene: NormalDraw", 0xffffff);
#endif
}

void MiniGameScene::FadeDraw()
{
	// 通常の描画を行う
	NormalDraw();

	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
#ifdef _DEBUG
	DrawString(0, 0, L"MiniGameScene: FadeDraw", 0xffffff);
#endif
}

bool MiniGameScene::IsEndDescription() const
{
	// フレームカウンタが説明を表示する時間を超えていたらtrueを返す
	return m_frameCount >= kDescriptionInterval;
}

void MiniGameScene::GetBalloonNum() const
{
}

void MiniGameScene::SetEventFunc()
{
	// -----------------------------------------------------
	// イベントセンサーの関数
	// -----------------------------------------------------


	// 指定されたアイテムが取得されたかどうかの関数を定義
	m_pEventSensors->isGetItemFunc = [this](const std::string& id)
		{
			auto type = Types::ItemType::Coin;
			if (id == "coin") // テスト用
			{
				type = Types::ItemType::Coin;
			}
			if (id == "medal")
			{
				type = Types::ItemType::UpgradeMedal;
			}
			if (id == "toitem")
			{
				type = Types::ItemType::ChangeToCoin;
			}
			if (id == "balloon")
			{
				type = Types::ItemType::Balloon;
			}

			return m_pGameManager->IsItemPicked(type);
		};

	// OKボタンが押されたかどうかの関数を定義
	m_pEventSensors->isPressedButtonFunc = [this]()
		{
			return m_isInputOK;
		};

	// カメラの補正が終わったかどうかの関数を定義
	m_pEventSensors->isCameraLerpEndFunc = [this]()
		{
			return m_pCamera->IsLerpEnd();
		};

	// 風船を指定数取得したかどうかの関数を定義
	m_pEventSensors->isGetBalloonFunc = [this](int balloonNum, bool isAll)
		{
			return m_pGameManager->IsGetBalloon(balloonNum, isAll);
		};

	// 風船の総数を取得する関数を定義
	m_pEventSensors->getTotalBalloonNumFunc = [this]()
		{
			return m_pGameManager->GetTotalBalloonNum();
		};

	// -----------------------------------------------------
	// イベントコントロールの関数
	// -----------------------------------------------------

	m_pEventControls->showHintFunc = [this](const std::string& id, const Size& size, const Position2& pos, float duration)
		{
			// ページ付きのテキストウィンドウを作成
			auto pages = m_pTextManager->GetAllPageText(id);
			// テキストウィンドウの内容を初期化(文字は最小サイズにする)
			auto ptr = m_pUIManager->CreateTextWindowPaged(id, pages, size, pos, duration, Types::FontType::Small);
			return ptr;
		};

	// テキストウィンドウを表示する関数を定義
	m_pEventControls->showTextWindowFunc = [this](const std::string& id, const Size& size, const Position2& pos, float duration)
		{
			// ページ付きのテキストウィンドウを作成
			auto pages = m_pTextManager->GetAllPageText(id);
			// テキストウィンドウの内容を初期化(文字は最小サイズにする)
			auto ptr = m_pUIManager->CreateTextWindowPaged(id, pages, size, pos, duration, Types::FontType::Small);
			return ptr;
		};

	// アイテムをドロップする関数を定義
	m_pEventControls->dropItemFunc = [this](int chestNo, const std::string& itemType)
		{
			if (itemType == "changecoin")
			{
				m_pGameManager->SpawnFloatingItem(kSpawnFlotingItemPos);
			}
			//Types::ItemType type; // 生成するアイテムが何かを判別する
			//// 宝箱の番号を取得
			//if (itemType == "coin")
			//{
			//	type = Types::ItemType::Coin;
			//}
			//if (itemType == "medal")
			//{
			//	type = Types::ItemType::UpgradeMedal;
			//}
			//if (itemType == "toitem")
			//{
			//	type = Types::ItemType::ChangeToCoin;
			//	// 通常のアイテム(コイン)をまず生成
			//	m_pGameManager->DropItem(m_pPositionRegistry->GetChestPos(chestNo), Types::ItemType::Coin);
			//}

			//m_pGameManager->DropItem(m_pPositionRegistry->GetChestPos(chestNo), type);
		};

	// 敵を指定座標にスポーンさせる関数を定義
	m_pEventControls->spawnEnemiesFunc = [this](const std::vector<Position2>& pos, int formNo)
		{
			for (const auto& enemyPos : pos)
			{
				m_pGameManager->SpawnEnemy(enemyPos, formNo);
			}
		};

	// プレイヤーのフリーズ状態を変更する関数を定義
	m_pEventControls->changePlayerFreezeFunc = [this]()
		{
			m_pGameManager->ChangeFreezePlayer();
		};

	// カメラが見るターゲットを変更する関数を定義
	m_pEventControls->lookCameraFunc = [this](const std::string& key)
		{
			/*if (key == "spawnPos")
			{
				m_pCamera->SetTarget(m_pPositionRegistry->GetCameraPos(kSpawnPosChipNo));
			}
			else if (key == "goalPos")
			{
				m_pCamera->SetTarget(m_pPositionRegistry->GetCameraPos(kGoalPosChipNo));
			}*/
		};

	// カメラが見るターゲットをプレイヤーに戻す関数を定義
	m_pEventControls->returnCameraFunc = [this]()
		{
			m_pCamera->SetTargetProvider([this]() {return m_pGameManager->GetPlayerPos(); });
		};

	// ゴールをアクティブにする関数を定義
	m_pEventControls->activeGoalFunc = [this]()
		{
			m_pMap->SetGoalMapChipOpen();
			m_pGameManager->ActiveGoal();
		};

}
