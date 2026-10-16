#include "GameScene.h"
#include "ResultScene.h"
#include "PauseScene.h"
#include "GameoverScene.h"
#include "LoadingScene.h"
#include "../Utility/Input.h"
#include "SceneController.h"
#include "../Utility/File/FileManager.h"
#include "../Utility/File/ImageFile.h"
#include "../Utility/CSV/PlayerAttackResource.h"
#include "../Utility/CSV/TextResource.h"
#include "../Utility/CSV/EventResource.h"
#include "../Object/Player/Player.h"
#include "../Object/Enemy/EnemyManager.h"
#include "../Object/Item/ItemManager.h"
#include "../Object/Camera/PlayerCamera.h"
#include "../Object/UI/LockOnMarkerUI.h"
#include "../Object/UI/LifeUI.h"
#include "../Object/UI/ParamUI.h"
#include "../Object/UI/BoardUI.h"
#include "../Object/Stage.h"
#include "../Object/Skybox.h"
#include "../MyLib/MyMath.h"
#include "../MyLib/MyString.h"
#include "../MyLib/Renderer.h"
#include "../MyLib/ObjectFactory.h"
#include "../MyLib/ObjectManager.h"
#include "../MyLib/Component/Transform.h"
#include "../MyLib/Component/Draw/Drawable3D.h"
#include "../MyLib/Component/Draw/UI/UITelop.h"
#include "../Main/Application.h"
#include "../Utility/Game.h"
#include "../Common/Sound/SoundManager.h"
#include "../Common/Event/EventSensors.h"
#include "../Common/Event/EventControls.h"
#include "../Common/Event/EventManager.h"
#include "../Common/Event/EventStructs.h"
#include "../Common/ComboCounter.h"
#include "../Common/ScoreCounter.h"
#include "../Common/ResultData.h"
#include "DxLib.h"
#include <cassert>

namespace
{
	// シーン遷移関連
	constexpr int kFadeInterval = 60; // フェードを行う時間
	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値
	constexpr unsigned int kFadeInColor = 0xffffff; // フェードインの色
	constexpr unsigned int kFadeOutColor = 0xffffff; // フェードアウトの色

	const Vector2Int kTelopPos = { Game::kScreenWidth / 2, Game::kScreenHeight / 2 };
	const Vector2Int kTextPos = { Game::kScreenWidth / 2, 50 };

	const Vector2Int kScoreTextPos = { Game::kScreenWidth - 200, 50 };

	const Vector2Int kComboTextPos = { Game::kScreenWidth - 200, 130 };

	const Vector3 kLightDir = { 0.0f,-1.5f,0.0f };
	const Vector3 kShadowMapAreaMin = { -61.0f,-1.0f,-65.0f };
	const Vector3 kShadowMapAreaMax = { 61.0f,20.0f,65.0f };

	// ライトの環境光の色
	const COLOR_F kLightAmbColor = { 10.0f, 10.0f, 10.0f, 10.0f };

	// ゲーム開始時のカウントダウン関連
	constexpr int kCountDownStartNum = 3; // カウントダウンを始める数字
	constexpr int kCountDownFrameParCount = Game::kFrameRate; // 1カウントにかけるフレーム数
	constexpr int kCountDownStartHoldFrame = 45; // "START!"を表示し続けるフレーム数

	// ロックオンを行う最大距離
	constexpr float kLockOnMaxDistance = 15.0f;

	constexpr float kLockOnStickThreshold = 0.7f;

	const Vector2Int kLifeUILeft = { 70, 60 };

	constexpr int kLifeUIMargin = 80;

	// 敵を倒した際に加算するスコア
	constexpr int kEnemyScore = 1000;
	// アイテムを獲得した際に加算するスコア
	constexpr int kItemScore = 500;

	// 操作説明のボード関連(チュートリアルステージでのみ表示する)
	const Vector2Int kGuideBoardSize = { 470, 390 }; // 見出しと操作説明8行分が収まるサイズ
	constexpr int kGuideBoardMargin = 20; // 画面の端からボードまでの余白
	// 画面の右下に表示する
	const Vector2Int kGuideBoardPos = { Game::kScreenWidth - kGuideBoardSize.x / 2 - kGuideBoardMargin, Game::kScreenHeight - kGuideBoardSize.y / 2 - kGuideBoardMargin };

	// 操作説明の1行分の内容
	struct GuideItem
	{
		const wchar_t* input;	// 操作(行の左側に表示する)
		const wchar_t* action;	// 行動(行の右側に表示する)
	};

	// 操作説明に表示する項目(上から順に表示する)
	const GuideItem kGuideItems[] =
	{
		{ L"左スティック", L"移動" },
		{ L"右スティック", L"カメラ視点操作" },
		{ L"Aボタン", L"攻撃" },
		{ L"Bボタン", L"ローリング" },
		{ L"Xボタン", L"カメラリセット" },
		{ L"RB", L"盾を構える" },
		{ L"LT(敵の近くで)", L"ロックオン" },
		{ L"STARTボタン", L"ポーズ" },
	};


	// ファイルを読み込む際のパスの最大サイズ(文字数)
	constexpr size_t kFilePathMax = 256;
}

GameScene::GameScene(SceneController& controller, int stageNo) :
	SceneBase(controller),
	m_update(&GameScene::FadeInUpdate),
	m_draw(&GameScene::FadeDraw),
	m_gameTime(0), 
	m_stageNo(stageNo),
	m_isGameStart(false),
	m_isLockOnChange(true)
{
}

GameScene::~GameScene()
{
	// 念のため終了処理を呼ぶようにする
	//End();
}

void GameScene::Init()
{
	m_frameCount = kFadeInterval;
	m_fadeColor = kFadeInColor;

	m_pPlayer = std::dynamic_pointer_cast<Player>(MyLib::ObjectFactory::CreatePlayer());

	m_pPlayer->Init();

	m_pEnemyManager = std::make_shared<EnemyManager>();
	m_pEnemyManager->Init(m_stageNo);
	m_pEnemyManager->CreateEnemy(m_pPlayer);

	m_pItemManager = std::make_shared<ItemManager>();
	m_pItemManager->Init(m_stageNo);

	m_pStage = std::make_shared<Stage>();

	m_pStage->Init(m_stageNo);

	m_pSkybox = std::dynamic_pointer_cast<Skybox>(MyLib::ObjectFactory::CreateSkybox());

	m_pSkybox->Init(Skybox::Type::Morning);

	m_pCamera = std::dynamic_pointer_cast<PlayerCamera>(MyLib::ObjectFactory::CreateCamera());

	m_pCamera->Init();
	m_pCamera->SetTarget(m_pPlayer->GetComponent<MyLib::Transform>());

	if (m_textResources.Load(L"Data/File/CSV/text_data.csv"))
	{
		m_textResources.ConvertTextData();
	}

	// チュートリアルステージでのみ操作説明を表示する
	// UIは生成した順に描画されるため、テロップが操作説明の上に表示されるようにテロップより先に生成する
	if (m_stageNo == Game::kTutorialStageNo)
	{
		m_pGuideBoard = std::make_shared<BoardUI>();
		m_pGuideBoard->Init(kGuideBoardPos, kGuideBoardSize);
		m_pGuideBoard->SetBackGround(FileManager::GetInstance().GetImage(L"Data/File/Image/dialog_back_green.png", false));
		m_pGuideBoard->SetTitle(L"操作説明");
		for (const auto& item : kGuideItems)
		{
			m_pGuideBoard->AddParam(item.input, item.action);
		}
	}

	m_pUITelop =MyLib::ObjectFactory::CretateUITelop(kTelopPos, MyLib::Renderer::FontType::Midium);
	m_pUITelop->Init();
	m_pTelop = m_pUITelop->GetComponent<MyLib::UITelop>();

	m_pUICountDown = MyLib::ObjectFactory::CreateUICountDown(kTelopPos, MyLib::Renderer::FontType::Header);
	m_pUICountDown->Init();
	m_pCountDown = m_pUICountDown->GetComponent<MyLib::UICountDown>();

	m_pUIText = MyLib::ObjectFactory::CreateUIText(kTextPos, MyLib::Renderer::FontType::Large);
	m_pUIText->Init();
	m_pText = m_pUIText->GetComponent<MyLib::UIText>();

	m_pUIComboText = MyLib::ObjectFactory::CreateUICombo(kComboTextPos, MyLib::Renderer::FontType::Large);
	m_pUIComboText->Init();
	m_pComboText = m_pUIComboText->GetComponent<MyLib::UICombo>();

	m_pScoreUI = std::make_shared<ParamUI>();
	m_pScoreUI->Init(kScoreTextPos);
	m_pScoreUI->SetText(L"スコア");

	m_pLockOnMarker = std::make_shared<LockOnMarkerUI>();

	m_pLifeUI = std::make_shared<LifeUI>();
	m_pLifeUI->Init(kLifeUILeft, kLifeUIMargin, m_pPlayer->GetMaxLife());
	m_pLifeUI->SetLife(m_pPlayer->GetLife());

	m_pEventSensors = std::make_shared<EventSensors>();
	m_pEventControls = std::make_shared<EventControls>();
	SetEventFunc();

	m_pEventManager = std::make_unique<EventManager>();

	m_pComboCounter = std::make_unique<ComboCounter>();

	m_pScoreCounter = std::make_unique<ScoreCounter>();

	wchar_t eventFilePath[kFilePathMax];
	std::swprintf(eventFilePath, kFilePathMax, L"Data/File/CSV/Event/stage%d_data.csv", m_stageNo);

	EventResource eventResource;
	if (eventResource.Load(eventFilePath))
	{
		eventResource.ConvertEventData();
	}

	m_pEventManager->Init
	(
		eventResource.GetEventDatas(),
		m_pEventControls, m_pEventSensors
	);

	SetLightDirection(kLightDir);
	COLOR_F color = GetLightAmbColor();
	SetLightAmbColor(kLightAmbColor);
	MyLib::Renderer::GetInstance().SetShadowMapLightDir(kLightDir);
	MyLib::Renderer::GetInstance().SetShadowMapArea(kShadowMapAreaMin, kShadowMapAreaMax);

	auto& soundManager = SoundManager::GetInstance();
	soundManager.LoadSoundClip("TitleBGM", L"Data/File/Sound/BGM/title.ogg", SoundBus::BGM, 1.0f, true);
	soundManager.LoadSoundClip("SelectBGM", L"Data/File/Sound/BGM/select.mp3", SoundBus::BGM, 1.0f, true);
	soundManager.LoadSoundClip("GameBGM", L"Data/File/Sound/BGM/stage1.ogg", SoundBus::BGM, 1.0f, true);
	soundManager.LoadSoundClip("ResultBGM", L"Data/File/Sound/BGM/result.mp3", SoundBus::BGM, 1.0f, true);
	soundManager.LoadSoundClip("GameoverBGM", L"Data/File/Sound/BGM/gameover.mp3", SoundBus::BGM, 1.0f, true);
	soundManager.LoadSoundClip("OK", L"Data/File/Sound/SE/ok.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("Cursor", L"Data/File/Sound/SE/cursor.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("Open", L"Data/File/Sound/SE/open.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("LockOn", L"Data/File/Sound/SE/lock_on.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("TargetChange", L"Data/File/Sound/SE/target_change.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("Pause", L"Data/File/Sound/SE/pause.mp3", SoundBus::SE, 1.0f, false);
	// 暫定的な実装
	// 敵やアイテムがゲームシーン内から全てなくなった場合に、下記のSEが再生されない場合があるためゲームシーン側でもロードする
	soundManager.LoadSoundClip("Die", L"Data/File/Sound/SE/enemy_die.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("Coin", L"Data/File/Sound/SE/coin_get.mp3", SoundBus::SE, 1.0f, false);
}

void GameScene::End()
{
	OutputDebugStringA("End: Player start\n");
	m_pPlayer->End();
	OutputDebugStringA("End: Player done enemy start\n");

	m_pEnemyManager->End();
	OutputDebugStringA("End: enemy done stage start\n");

	m_pItemManager->End();

	m_pStage->End();
	OutputDebugStringA("End: stage done camera start\n");

	m_pCamera->End();
	OutputDebugStringA("End: camera done  ui telop start\n");

	m_pUITelop->End();
	OutputDebugStringA("End: ui telop done countdown start\n");

	m_pUICountDown->End();
	OutputDebugStringA("End: countdown done UItext start\n");

	m_pUIText->End();
	OutputDebugStringA("End: UItext done skybox start\n");

	m_pScoreUI->End();

	if (m_pGuideBoard)
	{
		m_pGuideBoard->End();
	}

	m_pSkybox->End();
	OutputDebugStringA("End: skybox done \n");

	auto& soundManager = SoundManager::GetInstance();
	soundManager.DeleteSoundClip("TitleBGM");
	soundManager.DeleteSoundClip("SelectBGM");
	soundManager.DeleteSoundClip("GameBGM");
	soundManager.DeleteSoundClip("ResultBGM");
	soundManager.DeleteSoundClip("GameoverBGM");
	soundManager.DeleteSoundClip("OK");
	soundManager.DeleteSoundClip("Cursor");
	soundManager.DeleteSoundClip("Open");
	soundManager.DeleteSoundClip("LockOn");
	soundManager.DeleteSoundClip("TargetChange");
	soundManager.DeleteSoundClip("Pause");
	soundManager.DeleteSoundClip("Die");
	soundManager.DeleteSoundClip("Coin");
}

void GameScene::Update()
{
	m_pEventManager->Update();

	(this->*m_update)();
}

void GameScene::Draw() const
{
	(this->*m_draw)();

	//DrawGraph(0, 0, m_tex->GetHandle(), true);

	//DrawString(0, 0, L"GameScene", GetColor(255, 255, 255));
	//DrawFormatString(0, 16, GetColor(255, 255, 255), L"FRAME:%d", m_frameCount);
}

void GameScene::FadeInUpdate()
{
	m_pCamera->SetTarget(m_pPlayer->GetComponent<MyLib::Transform>());

	m_frameCount--;
	if (m_frameCount <= 0)
	{
		m_update = &GameScene::NormalUpdate;
		m_draw = &GameScene::NormalDraw;
		return;
	}
}

void GameScene::FadeOutUpdate()
{
	m_frameCount++;
	if (m_frameCount >= kFadeInterval)
	{
		//m_sceneController.ChangeScene(std::make_shared<ResultScene>(m_sceneController, m_gameTime));
		SoundManager::GetInstance().CrossFadeBGM("ResultBGM", 1.0f);
		// リザルトシーンに送るデータを取得する
		std::shared_ptr<ResultData> resultData = std::make_shared<ResultData>();
		resultData->stageNo = m_stageNo;						// ステージ番号
		resultData->score = m_pScoreCounter->GetScore();		// スコア
		resultData->clearTime = m_gameTime;						// クリアタイム
		resultData->life = m_pPlayer->GetLife();				// 残り体力
		resultData->maxCombo = m_pComboCounter->GetMaxCount();	// 最大コンボ
		// オブジェクト管理クラスに終了を通知する
		MyLib::ObjectManager::GetInstance().End();
		m_sceneController.RequestChangeScene(std::make_shared<LoadingScene>([&controller = m_sceneController, resultData, this] { return std::make_shared<ResultScene>(controller, resultData); }, L"Data/File/CSV/Resource/result_scene.csv", m_sceneController, LoadingScene::TransitionType::Change));
		return;
	}
}

void GameScene::NormalUpdate()
{
	m_pLifeUI->SetLife(m_pPlayer->GetLife());

	if (m_pPlayer->IsDied())
	{
		SoundManager::GetInstance().CrossFadeBGM("GameoverBGM", 1.0f);
		m_sceneController.PushScene(std::make_shared<GameoverScene>(m_sceneController, m_stageNo));
		return;
	}

	Input& input = Input::GetInstance();

	if (m_isGameStart)
	{
		m_gameTime++;

#ifdef _DEBUG

#else
		if (input.IsTriggered("Pause"))
		{
			SoundManager::GetInstance().Play("Pause", 1.0f, true);
			m_sceneController.PushScene(std::make_shared<PauseScene>(m_sceneController));
			return;
		}
#endif
	}

	if (auto text = m_pText.lock())
	{
		text->SetText(L"Time : " + std::to_wstring(m_gameTime / Game::kFrameRate));
	}

	//m_pCamera->SetTarget(m_pPlayer->GetComponent<MyLib::Transform>());

	if (input.IsTriggered("XButton") && !m_pCamera->IsLockOn())
	{
		m_pCamera->LookForward();
	}

	Input::XInputData stickData = input.GetXInputData();

	// Lトリガーが押されたら
	if (input.IsTriggeredXInput(false))
	{
		auto target = m_pEnemyManager->GetNearEnemyTransform(m_pPlayer->GetPos(), m_pCamera->GetEyePos(), m_pCamera->GetFovDegree());
		if (auto pTarget = target.lock())
		{
			SoundManager::GetInstance().Play("LockOn", 1.0f, true);

			m_pCamera->SetLockOnTarget(pTarget);
			// Playerにターゲットをセットする
			m_pPlayer->SetLockOnTarget(pTarget);
			m_pLockOnTarget = target;
			m_pLockOnMarker->Show(pTarget);
		}
	}

	m_pLockOnMarker->Update();

	if (auto pLockOnTarget = m_pLockOnTarget.lock())
	{
		// 右スティックの入力がロックオンを変更する値を超えた場合
		if (fabsf(stickData.rightStick.x) > kLockOnStickThreshold)
		{
			if (m_isLockOnChange)
			{
				bool isLeft = stickData.rightStick.x <= 0.0f;

				auto newTarget = m_pEnemyManager->ReGetNearEnemyTransform(m_pPlayer->GetPos(), m_pCamera->GetEyePos(), m_pCamera->GetFovDegree(), pLockOnTarget->GetPos(), isLeft);

				// 新たなターゲットが見つかった場合新しいものに変更する
				if (auto pNewTarget = newTarget.lock())
				{
					SoundManager::GetInstance().Play("TargetChange", 1.0f, true);

					m_pCamera->SetLockOnTarget(pNewTarget);
					// Playerにターゲットをセットする
					m_pPlayer->SetLockOnTarget(pNewTarget);
					m_pLockOnTarget = pNewTarget;
					m_pLockOnMarker->Show(pNewTarget);
				}

				m_isLockOnChange = false;
			}
		}
		else
		{
			m_isLockOnChange = true;
		}

		bool shouldRelease = false;

		// Lトリガーが離されたら
		if(input.IsReleasedXInput(false))
		{
			shouldRelease = true;
		}
		else if (Vector3::GetDistance(m_pPlayer->GetPos(), pLockOnTarget->GetPos()) > kLockOnMaxDistance)
		{
			shouldRelease = true;
		}

		if (shouldRelease)
		{
			m_pCamera->ClearLockOn();
			// Playerのターゲット解除
			m_pPlayer->ClearLockOn();
			m_pLockOnTarget.reset();
			m_pLockOnMarker->Hide();
		}
	}
	else if(m_pCamera->IsLockOn())
	{
		m_pCamera->ClearLockOn();
		// Playerのターゲット解除
		m_pPlayer->ClearLockOn();
	}
	else
	{
		m_pLockOnMarker->Hide();
	}

	m_pEnemyManager->Update();

	m_pItemManager->Update();

	for (int i = 0; i < m_pEnemyManager->GetDeadCountThisFrame(); ++i)
	{
		m_pComboCounter->AddKill();
		m_pScoreCounter->AddScore(m_pComboCounter->GetCount() * kEnemyScore);
	}

	for (int i = 0; i < m_pItemManager->GetAcquisitionCountThisFrame(); ++i)
	{
		m_pScoreCounter->AddScore(kItemScore);
	}

	m_pComboCounter->Update();

	m_pScoreCounter->Update();

	if (auto combo = m_pComboText.lock())
	{
		combo->SetText(std::to_wstring(m_pComboCounter->GetCount()) + L"Combo!!");
	
		if (m_pComboCounter->GetRemainRate() > 0.0f)
		{
			combo->SetActive(true);
			combo->SetParam(m_pComboCounter->GetCount(), m_pComboCounter->GetRemainRate());
		}
		else
		{
			combo->SetActive(false);
		}
	}

	m_pScoreUI->SetParam(m_pScoreCounter->GetDisplayedScore());
}

void GameScene::FadeDraw() const
{
	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameScene::NormalDraw() const
{
}

void GameScene::SetEventFunc()
{
	m_pEventSensors->isTextFinishedFunc = [this]()
		{
			// 敵を倒せ！のようなテキストの一連の流れが終わったらtrueになるようにしたい

			auto pTelop = m_pTelop.lock();
			return pTelop ? pTelop->IsSequenceFinished() : true;
		};

	m_pEventSensors->isCountDownFinishedFunc = [this]()
		{
			// カウントダウンが終わったらtrueになるようにしたい
			auto pCountDown = m_pCountDown.lock();

			return pCountDown ? pCountDown->IsFinished() : true;
		};

	m_pEventSensors->isAllEnemyDeadFunc = [this]()
		{
			return m_pEnemyManager->IsEnemyDeadAll();
		};

	m_pEventSensors->isAllItemGetFunc = [this]()
		{
			return m_pItemManager->IsItemGetAll();
		};

	m_pEventControls->showTextFunc = [this](const std::wstring& id)
		{
			// 敵を倒せ！のようなテキストの描画を始めるようにしたい
			auto pTelop = m_pTelop.lock();
			if (!pTelop) return;

			pTelop->ShowMessage(m_textResources.GetText(id));
		};

	m_pEventControls->startCountDownFunc = [this]()
		{
			// カウントダウンのテキストを始めるようにしたい
			auto pCountDown = m_pCountDown.lock();
			if (!pCountDown) return;
			pCountDown->StartCountDown(kCountDownStartNum, kCountDownFrameParCount, kCountDownStartHoldFrame);
		};

	m_pEventControls->setPlayerCanMoveFunc = [this](bool canMove)
		{
			m_pPlayer->SetCanAct(canMove);
			if (canMove)
			{
				m_isGameStart = true;
			}
		};

	m_pEventControls->setEnemyCanActFunc = [this](bool canAct)
		{
			m_pEnemyManager->SetCanAct(canAct);
		};

	m_pEventControls->goToClearSceneFunc = [this]()
		{
			m_update = &GameScene::FadeOutUpdate;
			m_draw = &GameScene::FadeDraw;
		};
}
