#include "ResultScene.h"
#include "TitleScene.h"
#include "SelectScene.h"
#include "GameScene.h"
#include "LoadingScene.h"
#include "SceneController.h"
#include "../Utility/Input.h"
#include "../Common/Model.h"
#include "../Geometry/Vector3.h"
#include "../Utility/Binary/TerrainResource.h"
#include "../Utility/File/FileManager.h"
#include "../Utility/Game.h"
#include "../MyLib/ObjectFactory.h"
#include "../MyLib/ObjectManager.h"
#include "../MyLib/MyMath.h"
#include "../Common/ResultData.h"
#include "../Common/Effect/EffectManager.h"
#include "../Common/Sound/SoundManager.h"
#include "../Object/Skybox.h"
#include "../Object/UI/ParamUI.h"
#include "../Object/UI/BoardUI.h"
#include "../Main/Application.h"
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

	constexpr float kParamAddRate = 0.2f;

	constexpr float kParamThreshold = 0.9f;

	// タイムボーナスの最大値
	constexpr int kTimeBonusMax = 5000;

	// タイムボーナスがもらえるクリアタイムの上限(秒)
	constexpr int kTimeBonusLimitSec = 30;

	// 体力1つにつき与えられるボーナススコア
	constexpr int kLifeBonusScore = 3000;

	// 最大コンボ数に応じて与えられるボーナススコア
	constexpr int kMaxComboBonusScore = 500;

	const Vector2Int kTextPos = { Game::kScreenWidth / 2, 80 };
	const Vector2Int kTextTimePos = { Game::kScreenWidth / 2, 200 };
	const Vector2Int kTextLifePos = { Game::kScreenWidth / 2, 200+75 };
	const Vector2Int kTextMaxComboPos = { Game::kScreenWidth / 2, 200+75+75 };
	const Vector2Int kTextScorePos = { Game::kScreenWidth / 2, 200 + 75 + 75 + 75 };

	// ボーナススコアを表示する位置のマージン
	const Vector2Int kTextBonusPosMargin = { 330, 0 };

	const Vector2Int kSelectListPos = { Game::kScreenWidth / 2, Game::kScreenHeight - 150 };
	const Vector2Int kSelectListSize = { 500, 200 };

	const Vector2Int kHighScoreBoardPos = { Game::kScreenWidth - 200, Game::kScreenHeight - 180 };
	const Vector2Int kHighScoreBoardSize = { 340, 190 };
	// ハイスコアのボードの、ステージ名とスコアの境目のずらし量
	// ステージ名の方が長いため、はみ出さないように境目を右にずらす
	constexpr int kHighScoreSeparatorOffsetX = 70;

	// 見出しの文字色
	constexpr unsigned int kHeaderTextColor = 0xffd400;

	// デバッグ表示関連
	constexpr unsigned int kDebugTextColor = 0xffffff; // デバッグ表示の文字色
	constexpr int kDebugTextLineHeight = 16; // デバッグ表示の1行の高さ


}

ResultScene::ResultScene(SceneController& controller, std::shared_ptr<ResultData> data) : 
	SceneBase(controller),
	m_update(&ResultScene::FadeInUpdate),
	m_draw(&ResultScene::FadeDraw),
	m_bonusScore{},
	m_prevHighScore(0)
{
	m_pResultData = data;
}

ResultScene::~ResultScene()
{
	// 念のため終了処理を呼ぶようにする
	//End();
}

void ResultScene::Init()
{
	m_frameCount = kFadeInterval;
	m_fadeColor = kFadeInColor;
	//printfDx("ResultScene:初期化");

	m_pSkybox = std::dynamic_pointer_cast<Skybox>(MyLib::ObjectFactory::CreateSkybox());
	m_pSkybox->Init(Skybox::Type::Evening);

	m_pUIText = MyLib::ObjectFactory::CreateUIText(kTextPos, MyLib::Renderer::FontType::Header);
	m_pUIText->Init();
	m_pText = m_pUIText->GetComponent<MyLib::UIText>();
	if (auto text = m_pText.lock())
	{
		text->SetTextColor(kHeaderTextColor);
		text->SetText(L"クリア!");
	}

	m_pTimeUI = std::make_shared<ParamUI>();
	m_pTimeUI->Init(kTextTimePos);
	m_pTimeUI->SetText(L"クリアタイム");

	m_pLifeUI = std::make_shared<ParamUI>();
	m_pLifeUI->Init(kTextLifePos);
	m_pLifeUI->SetText(L"残り体力");
	
	m_pMaxComboUI = std::make_shared<ParamUI>();
	m_pMaxComboUI->Init(kTextMaxComboPos);
	m_pMaxComboUI->SetText(L"最大コンボ");

	m_pScoreUI = std::make_shared<ParamUI>();
	m_pScoreUI->Init(kTextScorePos, MyLib::Renderer::FontType::Large);
	m_pScoreUI->SetText(L"スコア");

	m_pTimeBonusUI = std::make_shared<ParamUI>();
	m_pTimeBonusUI->Init(kTextTimePos + kTextBonusPosMargin, MyLib::Renderer::FontType::Small);
	m_pTimeBonusUI->SetBetweenSymbol(L"+");
	m_pTimeBonusUI->SetText(L"スコアボーナス! ");

	m_pLifeBonusUI = std::make_shared<ParamUI>();
	m_pLifeBonusUI->Init(kTextLifePos + kTextBonusPosMargin, MyLib::Renderer::FontType::Small);
	m_pLifeBonusUI->SetBetweenSymbol(L"+");
	m_pLifeBonusUI->SetText(L"スコアボーナス! ");

	m_pComboBonusUI = std::make_shared<ParamUI>();
	m_pComboBonusUI->Init(kTextMaxComboPos + kTextBonusPosMargin, MyLib::Renderer::FontType::Small);
	m_pComboBonusUI->SetBetweenSymbol(L"+");
	m_pComboBonusUI->SetText(L"スコアボーナス! ");

	m_pUISelectList = MyLib::ObjectFactory::CreateUISelectList(kSelectListPos, MyLib::Renderer::FontType::Small);
	m_pUISelectList->Init();
	m_pSelectList = m_pUISelectList->GetComponent<MyLib::UISelectList>();
	if (auto selectList = m_pSelectList.lock())
	{

		// クリアしたステージが最後のステージだった場合は次のステージに進めないようにする
		if (m_pResultData->stageNo >= Game::kStageNum)
		{
			selectList->SetSize(kSelectListSize);
			selectList->SetBackGroundHandle(FileManager::GetInstance().GetImage(L"Data/File/Image/dialog_back_green.png", false));

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
		else
		{
			selectList->SetSize(kSelectListSize);
			selectList->SetBackGroundHandle(FileManager::GetInstance().GetImage(L"Data/File/Image/dialog_back_green.png", false));

			selectList->AddOption(L"次のステージへ", [this]()
				{
						// 次のステージへ移行
						m_sceneController.ChangeScene(std::make_shared<GameScene>(m_sceneController, ++m_pResultData->stageNo));
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
	}

	m_pHighScoreBoard = std::make_shared<BoardUI>();
	m_pHighScoreBoard->Init(kHighScoreBoardPos, kHighScoreBoardSize);
	m_pHighScoreBoard->SetBackGround(FileManager::GetInstance().GetImage(L"Data/File/Image/dialog_back_green.png", false));
	m_pHighScoreBoard->SetTitle(L"ハイスコア");
	m_pHighScoreBoard->SetSeparatorOffsetX(kHighScoreSeparatorOffsetX);
	std::vector<int> highScore = Application::GetInstance().GetHighScore();
	m_pHighScoreBoard->AddParam(L"チュートリアル", highScore[Game::kTutorialStageNo]);
	m_pHighScoreBoard->AddParam(L"ステージ1", highScore[Game::kStage1No]);
	m_pHighScoreBoard->AddParam(L"ステージ2", highScore[Game::kStage2No]);

	m_prevHighScore = highScore[m_pResultData->stageNo];

	m_pDisplayedResultData = std::make_shared<ResultData>();

	SetResultData();

	// タイムボーナスの計算(既定の秒数を超えるまではボーナスがもらえる)
	m_bonusScore.timeBonus = std::max(0, (kTimeBonusLimitSec - m_pResultData->clearTime / Game::kFrameRate) * kTimeBonusMax);
	// 残り体力ボーナスの計算
	m_bonusScore.lifeBonus = m_pResultData->life * kLifeBonusScore;
	// 最大コンボボーナスの計算
	m_bonusScore.comboBonus = m_pResultData->maxCombo * kMaxComboBonusScore;

	m_displayedBonusScore = m_bonusScore;

	auto& soundManager = SoundManager::GetInstance();
	soundManager.LoadSoundClip("TitleBGM", L"Data/File/Sound/BGM/title.ogg", SoundBus::BGM, 1.0f, true);
	soundManager.LoadSoundClip("SelectBGM", L"Data/File/Sound/BGM/select.mp3", SoundBus::BGM, 1.0f, true);
	soundManager.LoadSoundClip("GameBGM", L"Data/File/Sound/BGM/stage1.ogg", SoundBus::BGM, 1.0f, true);
	soundManager.LoadSoundClip("ResultBGM", L"Data/File/Sound/BGM/result.mp3", SoundBus::BGM, 1.0f, true);
	soundManager.LoadSoundClip("OK", L"Data/File/Sound/SE/ok.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("Cursor", L"Data/File/Sound/SE/cursor.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("Open", L"Data/File/Sound/SE/open.mp3", SoundBus::SE, 1.0f, false);
}

void ResultScene::End()
{
	//printfDx("ResultScene:終了");
	m_pSkybox->End();
	m_pUIText->End();
	m_pUISelectList->End();
	
	m_pScoreUI->End();
	m_pTimeUI->End();
	m_pLifeUI->End();
	m_pMaxComboUI->End();

	m_pTimeBonusUI->End();
	m_pLifeBonusUI->End();
	m_pComboBonusUI->End();

	m_pHighScoreBoard->End();

	auto& soundManager = SoundManager::GetInstance();
	soundManager.DeleteSoundClip("TitleBGM");
	soundManager.DeleteSoundClip("SelectBGM");
	soundManager.DeleteSoundClip("GameBGM");
	soundManager.DeleteSoundClip("ResultBGM");
	soundManager.DeleteSoundClip("OK");
	soundManager.DeleteSoundClip("Cursor");
	soundManager.DeleteSoundClip("Open");
}

void ResultScene::Update()
{
	(this->*m_update)();
}

void ResultScene::Draw() const
{
	(this->*m_draw)();
#ifdef _DEBUG
	DrawString(0, 0, L"ResultScene", kDebugTextColor);
	DrawFormatString(0, kDebugTextLineHeight, kDebugTextColor, L"FRAME:%d", m_frameCount);
#endif // _DEBUG

}

void ResultScene::FadeInUpdate()
{
	m_frameCount--;

	if (m_frameCount <= 0)
	{
		m_update = &ResultScene::NormalUpdate;
		m_draw = &ResultScene::NormalDraw;
		return;
	}
}

void ResultScene::FadeOutUpdate()
{
	m_frameCount++;

	if (m_frameCount >= kFadeInterval)
	{
		auto selectList = m_pSelectList.lock();

		if (selectList->IsMatchedCursor(L"次のステージへ"))
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

void ResultScene::NormalUpdate()
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

	if (m_pDisplayedResultData->score < m_pResultData->score)
	{
		MyLib::UpdateParam(m_pDisplayedResultData->score, m_pResultData->score);
	}
	else
	{
		if (m_displayedBonusScore.timeBonus > 0)
		{
			MyLib::UpdateParamAdjustment(m_pDisplayedResultData->score, m_displayedBonusScore.timeBonus, m_pDisplayedResultData->score + m_bonusScore.timeBonus);
		}
		else
		{
			if (m_displayedBonusScore.lifeBonus > 0)
			{
				MyLib::UpdateParamAdjustment(m_pDisplayedResultData->score, m_displayedBonusScore.lifeBonus, m_pDisplayedResultData->score + m_bonusScore.lifeBonus);
			}
			else
			{
				if (m_displayedBonusScore.comboBonus > 0)
				{
					MyLib::UpdateParamAdjustment(m_pDisplayedResultData->score, m_displayedBonusScore.comboBonus, m_pDisplayedResultData->score + m_bonusScore.comboBonus);
				}
			}
		}
	}

	// ハイスコアの場合更新する
	if(m_prevHighScore < m_pDisplayedResultData->score)
	{
		m_pHighScoreBoard->SetParam(m_pResultData->stageNo, m_pDisplayedResultData->score);
		Application::GetInstance().SetHighScore(m_pDisplayedResultData->score, m_pResultData->stageNo);
	}

	if (m_pDisplayedResultData->clearTime < m_pResultData->clearTime)
	{
		MyLib::UpdateParam(m_pDisplayedResultData->clearTime, m_pResultData->clearTime);
	}

	if (m_pDisplayedResultData->life < m_pResultData->life)
	{
		MyLib::UpdateParam(m_pDisplayedResultData->life, m_pResultData->life);
	}

	if (m_pDisplayedResultData->maxCombo < m_pResultData->maxCombo)
	{
		MyLib::UpdateParam(m_pDisplayedResultData->maxCombo, m_pResultData->maxCombo);
	}

	

	SetResultData();

	if (input.IsTriggered("OK"))
	{
		// 決定した際のSEを再生
		SoundManager::GetInstance().Play("OK", 1.0f, false);
		m_update = &ResultScene::FadeOutUpdate;
		m_draw = &ResultScene::FadeDraw;
		return;
	}
}

void ResultScene::FadeDraw() const
{
	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void ResultScene::NormalDraw() const
{
}

void ResultScene::SetResultData()
{
	// スコアを表示するUIの設定
	m_pScoreUI->SetParam(m_pDisplayedResultData->score);

	// クリアタイムを表示するUIの設定
	m_pTimeUI->SetParam(m_pDisplayedResultData->clearTime / Game::kFrameRate);

	// 残り体力を表示するUIの設定
	m_pLifeUI->SetParam(m_pDisplayedResultData->life);

	// 最大コンボを表示するUIの設定
	m_pMaxComboUI->SetParam(m_pDisplayedResultData->maxCombo);

	// それぞれのボーナススコアを表示するUIの設定
	m_pTimeBonusUI->SetParam(m_displayedBonusScore.timeBonus);
	m_pLifeBonusUI->SetParam(m_displayedBonusScore.lifeBonus);
	m_pComboBonusUI->SetParam(m_displayedBonusScore.comboBonus);
}
