#include "GameoverScene.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "SelectScene.h"
#include "LoadingScene.h"
#include "SceneController.h"
#include "../Utility/Game.h"
#include "../Utility/Input.h"
#include "../MyLib/Physics.h"
#include "../MyLib/ObjectFactory.h"
#include "../MyLib/ObjectManager.h"
#include "../MyLib/Component/Draw/UI/UIImage.h"
#include "../Common/Effect/EffectManager.h"
#include "../Utility/File/FileManager.h"
#include "../Common/Sound/SoundManager.h"
#include "DxLib.h"

namespace
{
	// シーン遷移関連
	constexpr int kFadeInterval = 60; // フェードを行う時間
	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値
	constexpr unsigned int kFadeInColor = 0xffffff; // フェードインの色
	constexpr unsigned int kFadeOutColor = 0xffffff; // フェードアウトの色

	constexpr int kCursorMoveIndex = 1;	// カーソルが動く値

	const Vector2Int kGameoverImagePos = { Game::kScreenWidth / 2, 220 };

	const Vector2Int kSelectListPos = { Game::kScreenWidth / 2, Game::kScreenHeight - 250 };
	const Vector2Int kSelectListSize = { Game::kScreenWidth / 3, 250 };
}

GameoverScene::GameoverScene(SceneController& controller, int stageNo) :
	SceneBase(controller),
	m_update(&GameoverScene::FadeInUpdate),
	m_draw(&GameoverScene::FadeDraw),
	m_stageNo(stageNo)
{
}

GameoverScene::~GameoverScene()
{
}

void GameoverScene::Init()
{
	m_frameCount = kFadeInterval;
	m_fadeColor = kFadeInColor;

	auto gameoverImageFile = FileManager::GetInstance().GetImage(L"Data/File/Image/gameover.png", false);

	m_pUIGameOverImage = MyLib::ObjectFactory::CreateUIImage(kGameoverImagePos);
	auto gameoverImage = m_pUIGameOverImage->GetComponent<MyLib::UIImage>().lock();
	gameoverImage->SetImageFile(gameoverImageFile);
	m_pUIGameOverImage->Init();

	m_pUISelectList = MyLib::ObjectFactory::CreateUISelectList(kSelectListPos, MyLib::Renderer::FontType::Small);
	m_pUISelectList->Init();
	m_pSelectList = m_pUISelectList->GetComponent<MyLib::UISelectList>();
	if (auto selectList = m_pSelectList.lock())
	{

		selectList->SetSize(kSelectListSize);

		selectList->AddOption(L"リトライ", [this]()
			{
				// 最後のゲームシーンに戻る
				MyLib::ObjectManager::GetInstance().End();
				m_sceneController.ResetScene(std::make_shared<GameScene>(m_sceneController, m_stageNo));
			});
		selectList->AddOption(L"ステージセレクトに戻る", [this]()
			{
				MyLib::ObjectManager::GetInstance().End();
				m_sceneController.ChangeScene(std::make_shared<LoadingScene>(
					[&controller = m_sceneController] {return std::make_shared<SelectScene>(controller); }, L"Data/File/CSV/Resource/select_scene.csv", m_sceneController, LoadingScene::TransitionType::Reset));

			});
		selectList->AddOption(L"タイトルに戻る", [this]()
			{
				MyLib::ObjectManager::GetInstance().End();
				m_sceneController.ChangeScene(std::make_shared<LoadingScene>(
					[&controller = m_sceneController] {return std::make_shared<TitleScene>(controller); }, L"Data/File/CSV/Resource/title_scene.csv", m_sceneController, LoadingScene::TransitionType::Reset));
			});
	}

	MyLib::ObjectManager::GetInstance().SetUpdate(false);

	MyLib::Physics::GetInstance().StopUpdate();

	EffectManager::GetInstance().StopUpdate();

	auto& soundManager = SoundManager::GetInstance();
	soundManager.LoadSoundClip("TitleBGM", L"Data/File/Sound/BGM/title.ogg", SoundBus::BGM, 1.0f, true);
	soundManager.LoadSoundClip("SelectBGM", L"Data/File/Sound/BGM/select.mp3", SoundBus::BGM, 1.0f, true);
	soundManager.LoadSoundClip("GameBGM", L"Data/File/Sound/BGM/stage1.ogg", SoundBus::BGM, 1.0f, true);
	soundManager.LoadSoundClip("ResultBGM", L"Data/File/Sound/BGM/result.mp3", SoundBus::BGM, 1.0f, true);
	soundManager.LoadSoundClip("GameoverBGM", L"Data/File/Sound/BGM/gameover.mp3", SoundBus::BGM, 1.0f, true);
	soundManager.LoadSoundClip("OK", L"Data/File/Sound/SE/ok.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("Cursor", L"Data/File/Sound/SE/cursor.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("Open", L"Data/File/Sound/SE/open.mp3", SoundBus::SE, 1.0f, false);
}

void GameoverScene::End()
{
	m_pUIGameOverImage->End();
	m_pUISelectList->End();

	MyLib::ObjectManager::GetInstance().SetUpdate(true);

	MyLib::Physics::GetInstance().StartUpdate();

	EffectManager::GetInstance().StartUpdate();

	auto& soundManager = SoundManager::GetInstance();
	soundManager.DeleteSoundClip("TitleBGM");
	soundManager.DeleteSoundClip("SelectBGM");
	soundManager.DeleteSoundClip("GameBGM");
	soundManager.DeleteSoundClip("ResultBGM");
	soundManager.DeleteSoundClip("GameoverBGM");
	soundManager.DeleteSoundClip("OK");
	soundManager.DeleteSoundClip("Cursor");
	soundManager.DeleteSoundClip("Open");
}

void GameoverScene::Update()
{
	(this->*m_update)();
}

void GameoverScene::Draw() const
{
	(this->*m_draw)();
}

void GameoverScene::FadeInUpdate()
{
	m_frameCount--;

	if (m_frameCount <= 0)
	{
		m_update = &GameoverScene::NormalUpdate;
		m_draw = &GameoverScene::NormalDraw;
		return;
	}
}

void GameoverScene::FadeOutUpdate()
{
	m_frameCount++;

	if (m_frameCount >= kFadeInterval)
	{
		auto selectList = m_pSelectList.lock();

		if (selectList->IsMatchedCursor(L"リトライ"))
		{
			SoundManager::GetInstance().CrossFadeBGM("GameBGM", 1.0f);
		}
		else if (selectList->IsMatchedCursor(L"ステージセレクトに戻る"))
		{
			SoundManager::GetInstance().CrossFadeBGM("SelectBGM", 1.0f);
		}
		else if (selectList->IsMatchedCursor(L"タイトルに戻る"))
		{
			SoundManager::GetInstance().CrossFadeBGM("TitleBGM", 1.0f);
		}
		selectList->TriggerSelect();
		return;
	}
}

void GameoverScene::NormalUpdate()
{
	Input& input = Input::GetInstance();

	if (input.IsTriggered("Up"))
	{
		SoundManager::GetInstance().Play("Cursor", 1.0f, true);
		auto selectList = m_pSelectList.lock();
		selectList->MoveCursor(-kCursorMoveIndex);
	}

	if (input.IsTriggered("Down"))
	{
		SoundManager::GetInstance().Play("Cursor", 1.0f, true);
		auto selectList = m_pSelectList.lock();
		selectList->MoveCursor(kCursorMoveIndex);
	}


	if (input.IsTriggered("OK"))
	{
		// 決定した際のSEを再生
		SoundManager::GetInstance().Play("OK", 1.0f, false);
		m_update = &GameoverScene::FadeOutUpdate;
		m_draw = &GameoverScene::FadeDraw;
		return;
	}
}

void GameoverScene::FadeDraw() const
{
	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameoverScene::NormalDraw() const
{
}