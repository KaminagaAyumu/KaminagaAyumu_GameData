#include "SelectScene.h"
#include "DebugScene.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "LoadingScene.h"
#include "SceneController.h"
#include "../Utility/Input.h"
#include "../Utility/File/FileManager.h"
#include "../Utility/File/ImageFile.h"
#include "../Common/Model.h"
#include "../Common/Sound/SoundManager.h"
#include "../Geometry/Vector3.h"
#include "../Utility/Binary/TerrainResource.h"
#include "../Utility/Game.h"
#include "../MyLib/ObjectFactory.h"
#include "../MyLib/ObjectManager.h"
#include "../Object/Skybox.h"
#include <cassert>
#include "DxLib.h"

namespace
{
	// シーン遷移関連
	constexpr int kFadeInterval = 60; // フェードを行う時間
	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値
	constexpr unsigned int kFadeInColor = 0xffffff; // フェードインの色
	constexpr unsigned int kFadeOutColor = 0xffffff; // フェードアウトの色

	constexpr int kCursorMoveIndex = 1;	// カーソルが動く値

	const Vector2Int kTextPos = { Game::kScreenWidth / 2, 80 };
	const Vector2Int kTextTimePos = { Game::kScreenWidth / 2, Game::kScreenHeight - 30 };

	// 本来は左に寄せて右側に何か書く予定
	//const Vector2Int kSelectListPos = { 400, Game::kScreenHeight - 300 };
	const Vector2Int kSelectListPos = { Game::kScreenWidth / 2, Game::kScreenHeight - 300 };
	const Vector2Int kSelectListSize = { Game::kScreenWidth / 2, Game::kScreenHeight - 220 };

	// 見出し・操作説明の文字色
	constexpr unsigned int kTextColor = 0xffdc00;

	// デバッグ表示関連
	constexpr unsigned int kDebugTextColor = 0xffffff; // デバッグ表示の文字色
	constexpr int kDebugTextLineHeight = 16; // デバッグ表示の1行の高さ
}

SelectScene::SelectScene(SceneController& controller) :
	SceneBase(controller),
	m_update(&SelectScene::FadeInUpdate),
	m_draw(&SelectScene::FadeDraw)
{
}


SelectScene::~SelectScene()
{
	// 念のため終了処理を呼ぶようにする
	//End();
}

void SelectScene::Init()
{
	m_frameCount = kFadeInterval;
	m_fadeColor = kFadeInColor;
	//printfDx("SelectScene:初期化");

	m_file = FileManager::GetInstance().GetImage(L"Data/File/Image/dialog.png", false);

	m_pSkybox = std::dynamic_pointer_cast<Skybox>(MyLib::ObjectFactory::CreateSkybox());
	m_pSkybox->Init(Skybox::Type::Night);

	m_pUIText = MyLib::ObjectFactory::CreateUIText(kTextPos, MyLib::Renderer::FontType::Header);
	m_pUIText->Init();
	m_pText = m_pUIText->GetComponent<MyLib::UIText>();
	if (auto text = m_pText.lock())
	{
		text->SetTextColor(kTextColor);
		text->SetText(L"ステージ選択");
	}

	m_pUITextTime = MyLib::ObjectFactory::CreateUIText(kTextTimePos, MyLib::Renderer::FontType::Midium);
	m_pUITextTime->Init();
	m_pTextTime = m_pUITextTime->GetComponent<MyLib::UIText>();
	if (auto text = m_pTextTime.lock())
	{
		text->SetTextColor(kTextColor);
		text->SetText(L"Aボタンで決定");
	}

	m_pUISelectList = MyLib::ObjectFactory::CreateUISelectList(kSelectListPos, MyLib::Renderer::FontType::Small);
	m_pUISelectList->Init();
	m_pSelectList = m_pUISelectList->GetComponent<MyLib::UISelectList>();
	if (auto selectList = m_pSelectList.lock())
	{

		selectList->SetSize(kSelectListSize);
		selectList->SetBackGroundHandle(m_file);

		selectList->AddOption(L"チュートリアル", [this]()
			{
				MyLib::ObjectManager::GetInstance().End();
				m_sceneController.ChangeScene(std::make_shared<LoadingScene>(
					[&controller = m_sceneController] {return std::make_shared<GameScene>(controller, Game::kTutorialStageNo); }, L"Data/File/CSV/Resource/game_scene.csv", m_sceneController, LoadingScene::TransitionType::Change));
			});
		selectList->AddOption(L"ステージ1", [this]()
			{
				MyLib::ObjectManager::GetInstance().End();
				m_sceneController.ChangeScene(std::make_shared<LoadingScene>(
					[&controller = m_sceneController] {return std::make_shared<GameScene>(controller, Game::kStage1No); }, L"Data/File/CSV/Resource/game_scene.csv", m_sceneController, LoadingScene::TransitionType::Change));
			});
		selectList->AddOption(L"ステージ2", [this]()
			{
				MyLib::ObjectManager::GetInstance().End();
				m_sceneController.ChangeScene(std::make_shared<LoadingScene>(
					[&controller = m_sceneController] {return std::make_shared<GameScene>(controller, Game::kStage2No); }, L"Data/File/CSV/Resource/game_scene.csv", m_sceneController, LoadingScene::TransitionType::Change));
			});
		selectList->AddOption(L"タイトルに戻る", [this]()
			{
				MyLib::ObjectManager::GetInstance().End();
				m_sceneController.ChangeScene(std::make_shared<LoadingScene>(
					[&controller = m_sceneController] {return std::make_shared<TitleScene>(controller); }, L"Data/File/CSV/Resource/title_scene.csv", m_sceneController, LoadingScene::TransitionType::Change));
			});
	}

	auto& soundManager = SoundManager::GetInstance();
	soundManager.LoadSoundClip("TitleBGM", L"Data/File/Sound/BGM/title.ogg", SoundBus::BGM, 1.0f, true);
	soundManager.LoadSoundClip("SelectBGM", L"Data/File/Sound/BGM/select.mp3", SoundBus::BGM, 1.0f, true);
	soundManager.LoadSoundClip("GameBGM", L"Data/File/Sound/BGM/stage1.ogg", SoundBus::BGM, 1.0f, true);
	soundManager.LoadSoundClip("OK", L"Data/File/Sound/SE/ok.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("Cursor", L"Data/File/Sound/SE/cursor.mp3", SoundBus::SE, 1.0f, false);

}

void SelectScene::End()
{
	//printfDx("SelectScene:終了");
	m_pSkybox->End();
	m_pUIText->End();
	m_pUITextTime->End();
	m_pUISelectList->End();

	auto& soundManager = SoundManager::GetInstance();
	soundManager.DeleteSoundClip("TitleBGM");
	soundManager.DeleteSoundClip("SelectBGM");
	soundManager.DeleteSoundClip("GameBGM");
	soundManager.DeleteSoundClip("OK");
	soundManager.DeleteSoundClip("Cursor");
}

void SelectScene::Update()
{
	(this->*m_update)();
}

void SelectScene::Draw() const
{
	(this->*m_draw)();
#ifdef _DEBUG
	DrawString(0, 0, L"SelectScene", kDebugTextColor);
	DrawFormatString(0, kDebugTextLineHeight, kDebugTextColor, L"FRAME:%d", m_frameCount);
#endif // _DEBUG

}

void SelectScene::FadeInUpdate()
{
	m_frameCount--;

	if (m_frameCount <= 0)
	{
		m_update = &SelectScene::NormalUpdate;
		m_draw = &SelectScene::NormalDraw;
		return;
	}
}

void SelectScene::FadeOutUpdate()
{
	m_frameCount++;

	if (m_frameCount >= kFadeInterval)
	{
		auto selectList = m_pSelectList.lock();
		selectList->TriggerSelect();
		return;
	}
}

void SelectScene::NormalUpdate()
{
	Input& input = Input::GetInstance();

	if (input.IsTriggered("Up"))
	{
		// カーソルが動く際のSEを再生
		SoundManager::GetInstance().Play("Cursor", 1.0f, true);
		auto selectList = m_pSelectList.lock();
		selectList->MoveCursor(-kCursorMoveIndex);
	}

	if (input.IsTriggered("Down"))
	{
		// カーソルが動く際のSEを再生
		SoundManager::GetInstance().Play("Cursor", 1.0f, true);
		auto selectList = m_pSelectList.lock();
		selectList->MoveCursor(kCursorMoveIndex);
	}


	if (input.IsTriggered("OK"))
	{
		// 決定した際のSEを再生
		SoundManager::GetInstance().Play("OK", 1.0f, false);
		SoundManager::GetInstance().CrossFadeBGM("GameBGM", 1.0f);
		m_update = &SelectScene::FadeOutUpdate;
		m_draw = &SelectScene::FadeDraw;

		return;
	}
}

void SelectScene::FadeDraw() const
{
	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SelectScene::NormalDraw() const
{
}