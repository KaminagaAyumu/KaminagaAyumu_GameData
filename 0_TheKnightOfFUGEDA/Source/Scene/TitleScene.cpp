#include "TitleScene.h"
#include "SelectScene.h"
#include "DebugScene.h"
#include "LoadingScene.h"
#include "SceneController.h"
#include "../Main/Application.h"
#include "../Object/Skybox.h"
#include "../Object/Player/Player.h"
#include "../Object/Camera/PlayerCamera.h"
#include "../Utility/Input.h"
#include "../Utility/File/FileManager.h"
#include "../Utility/File/ImageFile.h"
#include "../Utility/Game.h"
#include "../MyLib/ObjectFactory.h"
#include "../MyLib/ObjectManager.h"
#include "../MyLib/Component/Draw/UI/UIImage.h"
#include "../Geometry/Vector2Int.h"
#include "../Common/Sound/SoundManager.h"
#include "../Common/Effect/EffectManager.h"
#include "../Common/Effect/Effect.h"
#include "DxLib.h"

namespace
{
	// シーン遷移関連
	constexpr int kFadeInInterval = 180; // フェードを行う時間
	constexpr int kFadeOutInterval = 60; // フェードインの時間
	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値
	constexpr unsigned int kFadeInColor = 0x000000; // フェードインの色
	constexpr unsigned int kFadeOutColor = 0xffffff; // フェードアウトの色

	constexpr int kCursorMoveIndex = 1;	// カーソルが動く値

	const Vector2Int kTitlePos = { Game::kScreenWidth / 2, 250 };
	const Vector2Int kGameStartTextPos = { Game::kScreenWidth / 2, 600 };

	const Vector2Int kSelectListPos = { Game::kScreenWidth / 2, Game::kScreenHeight - 150 };
	const Vector2Int kSelectListSize = { 325, 180 };

	const Vector3 kEffectPos = { 0.0f, 0.0f, 15.0f };

	// UIの演出関連
	constexpr int kStartTextBlinkFrame = 120; // スタートテキストの点滅1周のフレーム数
	constexpr int kStartTextFadeOutFrame = 20; // スタートテキストが消えるまでのフレーム数
	constexpr int kSelectListAppearFrame = 10; // 選択リストの出現が終わるまでのフレーム数

	// デバッグ表示関連
	constexpr unsigned int kDebugTextColor = 0xffffff; // デバッグ表示の文字色
	constexpr int kDebugTextLineHeight = 16; // デバッグ表示の1行の高さ
}

TitleScene::TitleScene(SceneController& controller) : 
	SceneBase(controller),
	m_update(&TitleScene::FadeInUpdate),
	m_draw(&TitleScene::FadeDraw),
	m_fadeInterval(kFadeInInterval)
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Init()
{
	m_file = FileManager::GetInstance().GetImage(L"Data/File/Image/text_start.png", false);

	m_file2 = FileManager::GetInstance().GetImage(L"Data/File/Image/title.png", false);

	m_titleLogo = MyLib::ObjectFactory::CreateUIImage(kTitlePos);
	auto titleLogoUIImage = m_titleLogo->GetComponent<MyLib::UIImage>().lock();
	titleLogoUIImage->SetImageFile(m_file2);
	m_titleLogo->Init();

	m_startText = MyLib::ObjectFactory::CreateUIImage(kGameStartTextPos);
	auto pUIImage = m_startText->GetComponent<MyLib::UIImage>().lock();
	pUIImage->SetImageFile(m_file);
	pUIImage->StartBlinking(kStartTextBlinkFrame);

	m_skybox = std::dynamic_pointer_cast<Skybox>(MyLib::ObjectFactory::CreateSkybox());

	m_skybox->Init(Skybox::Type::Noon);

	m_startText->Init();

	
	m_pCamera = std::dynamic_pointer_cast<PlayerCamera>(MyLib::ObjectFactory::CreateCamera());

	m_pCamera->Init();
	
	m_frameCount = m_fadeInterval;
	m_fadeColor = kFadeInColor;

	m_pUISelectList = MyLib::ObjectFactory::CreateUISelectList(kSelectListPos, MyLib::Renderer::FontType::Small);
	m_pUISelectList->Init();
	m_pSelectList = m_pUISelectList->GetComponent<MyLib::UISelectList>();
	if (auto selectList = m_pSelectList.lock())
	{

		selectList->SetSize(kSelectListSize);
		selectList->SetBackGroundHandle(FileManager::GetInstance().GetImage(L"Data/File/Image/dialog_back_green.png", false));

		selectList->AddOption(L"ゲームスタート", [this]()
			{
				// ステージセレクトシーンに移行
				m_sceneController.ChangeScene(std::make_shared<LoadingScene>(
					[&controller = m_sceneController] {return std::make_shared<SelectScene>(controller); }, L"Data/File/CSV/Resource/select_scene.csv", m_sceneController, LoadingScene::TransitionType::Change));
			});
		selectList->AddOption(L"ゲーム終了", [this]()
			{
				// とりあえずそのまま終了させる
				MyLib::ObjectManager::GetInstance().End();
				Application::GetInstance().RequestGameEnd();
			});
		selectList->SetActive(false);
	}

	auto& soundManager = SoundManager::GetInstance();
	soundManager.LoadSoundClip("TitleBGM", L"Data/File/Sound/BGM/title.ogg", SoundBus::BGM, 1.0f, true);
	soundManager.LoadSoundClip("SelectBGM", L"Data/File/Sound/BGM/select.mp3", SoundBus::BGM, 1.0f, true);
	soundManager.LoadSoundClip("OK", L"Data/File/Sound/SE/ok.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("Cursor", L"Data/File/Sound/SE/cursor.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("Open", L"Data/File/Sound/SE/open.mp3", SoundBus::SE, 1.0f, false);

	EffectManager::GetInstance().LoadEffect(L"title_decoration.efk");

	m_pEffect = EffectManager::GetInstance().CreateEffect(L"title_decoration.efk", kEffectPos, Quaternion::Identity());

	SoundManager::GetInstance().PlayBGM("TitleBGM", 0.0f);
}

void TitleScene::End()
{
	if(auto effect = m_pEffect.lock())
	{
		effect->StopEffect();
	}

	m_pCamera->End();
	m_startText->End();
	m_titleLogo->End();
	m_skybox->End();
	m_pUISelectList->End();

	auto& soundManager = SoundManager::GetInstance();
	soundManager.DeleteSoundClip("TitleBGM");
	soundManager.DeleteSoundClip("SelectBGM");
	soundManager.DeleteSoundClip("OK");
	soundManager.DeleteSoundClip("Cursor");
	soundManager.DeleteSoundClip("Open");

	EffectManager::GetInstance().DeleteEffect(L"title_decoration.efk");
}

void TitleScene::Update()
{
	//m_frameCount++;

	(this->*m_update)();
}

void TitleScene::Draw() const
{
	(this->*m_draw)();
#ifdef _DEBUG
	DrawString(0, 0, L"GameScene", kDebugTextColor);
	DrawFormatString(0, kDebugTextLineHeight, kDebugTextColor, L"FRAME:%d", m_frameCount);
#endif // _DEBUG

}

void TitleScene::FadeInUpdate()
{
	m_frameCount--;

	if (m_frameCount <= 0)
	{
		m_update = &TitleScene::NormalUpdate;
		m_draw = &TitleScene::NormalDraw;
		return;
	}
}

void TitleScene::FadeOutUpdate()
{
	m_frameCount++;

	if (m_frameCount >= m_fadeInterval)
	{
		auto selectList = m_pSelectList.lock();
		selectList->TriggerSelect();
		return;
	}
}

void TitleScene::NormalUpdate()
{
	Input& input = Input::GetInstance();

	if (input.IsTriggered("OK"))
	{
		m_update = &TitleScene::SelectUpdate;
		auto selectList = m_pSelectList.lock();
		selectList->SetActive(true);
		selectList->StartAppearCenter(kSelectListAppearFrame);

		auto pUIImage = m_startText->GetComponent<MyLib::UIImage>().lock();
		if (pUIImage)
		{
			pUIImage->StopBlinking();
			pUIImage->StartFadeOut(kStartTextFadeOutFrame, false);
		}
		
		// ウィンドウを開く際のSEを再生
		SoundManager::GetInstance().Play("Open", 1.0f, true);

		return;
	}
}

void TitleScene::SelectUpdate()
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
		SoundManager::GetInstance().CrossFadeBGM("SelectBGM", 1.0f);
		m_update = &TitleScene::FadeOutUpdate;
		m_draw = &TitleScene::FadeDraw;
		m_fadeColor = kFadeOutColor;
		m_fadeInterval = kFadeOutInterval;
		return;
	}
}

void TitleScene::FadeDraw() const
{
	NormalDraw();

	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(m_fadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void TitleScene::NormalDraw() const
{
}
