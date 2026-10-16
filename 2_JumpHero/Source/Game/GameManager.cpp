#include "GameManager.h"
#include "Actor.h"
#include "Player.h"
#include "ChestManager.h"
#include "ItemManager.h"
#include "EnemyManager.h"
#include "Effect/EffectManager.h"
#include "../Utility/UI/UIManager.h"
#include "../Utility/UI/UIGauge.h"
#include "../Utility/UI/UIFormatText.h"
#include "../Utility/Camera.h"
#include "../Utility/Input.h"
#include "../Utility/Map.h"
#include "../Utility/Game.h"
#include "../Utility/Application.h"
#include "../Utility/Sound/SoundManager.h"
#include "DxLib.h"
#include <cassert>

namespace
{
	constexpr int kFirstLife = 3; // プレイヤーの残機初期化用

	constexpr int kPlayerNum = 1;

	constexpr int kEnemySpawnTime = 600; // 敵がスポーンするまでの時間

	constexpr float kGoalWidth = 40.0f;
	constexpr float kGoalHeight = 60.0f;

	constexpr float	kScoreAddRate = 0.2f; // スコア加算の割合
	constexpr float	kScoreThreshold = 0.9f; // スコア加算の閾値

	constexpr float kChangeToCoinBGMFadeTime = 30.0f; // 敵をコインに変えている時間のBGMのフェード値

	constexpr float kChangeToCoinSEVolume = 30.0f; // 敵をコインに変えた際のSEのボリューム
	constexpr float kNotPowerUpSEVolume = 12.0f; // プレイヤーがパワーアップ出来なかった時に鳴らすSEのボリューム
	constexpr float kPowerUpSEVolume = 40.0f; // プレイヤーがパワーアップした時に鳴らすSEのボリューム
	constexpr float kCoinGaugeMaxSEVolume = 50.0f; // ゲージがたまった時のSEのボリューム


	constexpr int kCoinAddScore = 1000; // コイン取得時のスコア
	constexpr int kBalloonAddScore = 100; // 風船取得時のスコア
	constexpr int kChangeToCoinAddScore = 100; // 敵をコインに変えるアイテム取得時のスコア
	constexpr int kUpgradeMedalAddScore = 100; // 強化アイテム取得時のスコア

	constexpr int kReadyGoTextTypeFrame = 5; // プレイヤーの登場する際の文字が出るスピード
	constexpr int kReadyGoTextWaitFrame = 60; // プレイヤーの登場する際の文字が完全に出てからフェードするまでの時間
	constexpr int kReadyGoTextFadeFrame = 60; // プレイヤーの登場する際の文字がフェードする時間

	constexpr int kMissTextTypeFrame = 3; // プレイヤーがミスになった際の文字が出るスピード
	constexpr int kMissTextWaitFrame = 120; // プレイヤーがミスになった際の文字が完全に出てからフェードするまでの時間
	constexpr int kMissTextFadeFrame = 30; // プレイヤーがミスになった際の文字がフェードする時間

	constexpr int kBalloonForChangeToCoin = 5; // 敵をコインに変えるアイテムを落とすために必要な風船の数
	constexpr int kLastStageBalloonForChangeToCoin = 15; // 最後のステージの敵をコインに変えるアイテムを落とすために必要な風船の数

	constexpr int kCoinTimeGaugeNum = 1; // 風船のゲージの番号
	const Size kCoinTimeGaugeSize = { 200, 40 }; // UIで使用するゲージのサイズ
	const Position2 kCoinTimeGaugePos = { 20.0f,180.0f }; // 風船のゲージの座標

	const Position2 kCoinGaugeIconPos = { 40.0f,80.0f }; // 風船の取得数アイコンの中心座標
}

GameManager::GameManager() :
	m_frameCount(0),
	m_score(0),
	m_currentScore(0),
	m_life(kFirstLife),
	m_medalNum(0),
	m_balloonNum(0),
	m_balloonCounter(0),
	m_totalBalloonNum(0),
	m_changeToCoinDropBalloonNum(kBalloonForChangeToCoin),
	m_miniGameBalloonCounterRate(0.0f),
	m_isMiniGame(false),
	m_isTutorial(false),
	m_isOpenGoal(false),
	m_isItemGaugeMax(false),
	m_isChangeToCoin(false)
{
	Application::GetInstance().GetSoundManager()->LoadSoundClip("coinSE", L"data/sound/SE/coinSE.wav", SoundBus::SE, 1.0f, false);
	Application::GetInstance().GetSoundManager()->LoadSoundClip("medalSE", L"data/sound/SE/medalSE.wav", SoundBus::SE, 1.0f, false);
	Application::GetInstance().GetSoundManager()->LoadSoundClip("balloonSE", L"data/sound/SE/balloonSE.mp3", SoundBus::SE, 1.0f, false);
	Application::GetInstance().GetSoundManager()->LoadSoundClip("changeToCoinSE", L"data/sound/SE/changeToCoinSE.mp3", SoundBus::SE, kChangeToCoinSEVolume, false);
	Application::GetInstance().GetSoundManager()->LoadSoundClip("dropChangeToCoinSE", L"data/sound/SE/dropChangeToCoinSE.mp3", SoundBus::SE, kChangeToCoinSEVolume, false);
	Application::GetInstance().GetSoundManager()->LoadSoundClip("coinGaugeMaxSE", L"data/sound/SE/coinGaugeMax.mp3", SoundBus::SE, kCoinGaugeMaxSEVolume, false);
	Application::GetInstance().GetSoundManager()->LoadSoundClip("powerUpSE", L"data/sound/SE/powerUp.mp3", SoundBus::SE, kPowerUpSEVolume, false);
	Application::GetInstance().GetSoundManager()->LoadSoundClip("notPowerUpSE", L"data/sound/SE/notPowerUp.mp3", SoundBus::SE, kNotPowerUpSEVolume, false);
	Application::GetInstance().GetSoundManager()->LoadSoundClip("changeToCoinBGM", L"data/sound/BGM/changeToCoinBGM.mp3", SoundBus::BGM, 1.0f, true);
	// アイテムを取った際のラムダ式定義
	// 風船を取った時
	m_itemCollectFunc[Types::ItemType::Balloon] = [this](const Position2& pos)
		{
			m_balloonNum++; // 風船の数を加算
			m_balloonCounter++; // 風船取得カウンターを加算
			AddScore(kBalloonAddScore);
			Application::GetInstance().GetSoundManager()->Play("balloonSE", 1.0f, true);
			auto manager = m_pEffectManager.lock();
			manager->CreateEffekseerEffect(Types::EffectType::BalloonGet, pos,true);
		};
	// 強化メダルを取った時
	m_itemCollectFunc[Types::ItemType::UpgradeMedal] = [this](const Position2& pos)
		{
			m_medalNum++; // メダルの数を加算
			AddScore(kUpgradeMedalAddScore);
			Application::GetInstance().GetSoundManager()->Play("medalSE", 1.0f, true);
			auto manager = m_pEffectManager.lock();
			manager->CreateEffekseerEffect(Types::EffectType::CoinGet, pos, true);
		};
	// 1UPを取った時
	m_itemCollectFunc[Types::ItemType::LifeUp] = [this](const Position2& pos)
		{
			m_life++; // 残機を加算
			auto manager = m_pEffectManager.lock();
			manager->CreateEffekseerEffect(Types::EffectType::CoinGet, pos, true);
		};
	// コインを取った時
	m_itemCollectFunc[Types::ItemType::Coin] = [this](const Position2& pos)
		{
			AddScore(kCoinAddScore); // スコアを加算
			Application::GetInstance().GetSoundManager()->Play("coinSE", 1.0f, true);
			auto manager = m_pEffectManager.lock();
			manager->CreateEffekseerEffect(Types::EffectType::CoinGet, pos, true);
		};
	// 敵をコインに変えるアイテムを取った時
	m_itemCollectFunc[Types::ItemType::ChangeToCoin] = [this](const Position2& pos)
		{
			AddScore(kChangeToCoinAddScore);
			ChangeEnemyToCoin(); // 敵をすべてアイテム化
			Application::GetInstance().GetSoundManager()->Play("changeToCoinSE", 1.0f, true);
			auto manager = m_pEffectManager.lock();
			manager->CreateEffekseerEffect(Types::EffectType::Star, pos, true);
		};
	m_itemCollectFunc[Types::ItemType::AttackItem] = [this](const Position2& pos)
		{
			// 攻撃アイテム取得時の処理
			m_pPlayer->AttackableStart();
			auto manager = m_pEffectManager.lock();
			manager->CreateEffekseerEffect(Types::EffectType::CoinGet, pos, true);
		};


}

GameManager::~GameManager()
{
}

void GameManager::Init(std::weak_ptr<Map> map, std::weak_ptr<Camera> camera, std::weak_ptr<EffectManager> effectManager, std::weak_ptr<UIManager> uiManager, std::vector<std::weak_ptr<Actor>>& actors, bool isTutorial)
{
	m_pMap = map;
	auto pMap = m_pMap.lock();
	m_pEffectManager = effectManager;
	m_pUIManager = uiManager;
	m_pPlayer = std::make_shared<Player>(map, weak_from_this());
	m_pPlayer->SetCamera(camera);
	m_pPlayer->Init();
	m_pChestManager = std::make_unique<ChestManager>(camera, this);
	m_pChestManager->SpawnChest(map);
	m_pChestManager->PushActors(actors);
	m_pItemManager = std::make_unique<ItemManager>(camera, weak_from_this());
	m_pItemManager->FirstSpawnItem(map);
	m_pItemManager->PushActors(actors);
	m_pEnemyManager = std::make_unique<EnemyManager>(camera, m_pPlayer.get(), weak_from_this(), map);

	m_life = kFirstLife;
	m_medalNum = 0;
	m_balloonNum = 0;
	m_balloonCounter = 0;
	m_totalBalloonNum = m_pItemManager->GetFirstBalloonNum();
	m_isMiniGame = false;
	m_isTutorial = isTutorial;
}

void GameManager::MiniGameInit(std::weak_ptr<Map> map, std::weak_ptr<Camera> camera, std::weak_ptr<EffectManager> effectManager, std::weak_ptr<UIManager> uiManager, std::vector<std::weak_ptr<Actor>>& actors)
{
	m_pMap = map;
	auto pMap = m_pMap.lock();
	m_pEffectManager = effectManager;
	m_pUIManager = uiManager;
	m_pPlayer->InitMap(pMap);
	m_pPlayer->SetCamera(camera);
	m_pPlayer->Init();
	m_pEnemyManager->Init(pMap,camera);
	m_pChestManager->Init(camera);
	m_pItemManager->Init(camera);
	m_pItemManager->FirstSpawnItem(map);
	m_pItemManager->PushActors(actors);

	m_balloonNum = 0; // 取得した風船の数をリセット
	m_balloonCounter = 0; // 風船カウンタをリセット
	m_totalBalloonNum = m_pItemManager->GetFirstBalloonNum(); // 風船の総数を改めて取得
	m_isMiniGame = true; // ミニゲームフラグをセット
	m_isOpenGoal = false; // ゴールのアクティブ状態をリセット
}

void GameManager::Update(Input& input)
{
#ifdef _DEBUG
	// printfを初期化するコマンド(上入力しながらLボタン)
	if (input.IsTriggered("LShift") && input.IsPressed("Up"))
	{
		clsDx();
	}
#endif
	m_frameCount++;

	// スコアの更新処理
	if (m_score < m_currentScore) // スコアが現在のスコアより小さい場合
	{
		int add = static_cast<int>((m_currentScore - m_score) * kScoreAddRate); // 現在のスコアとの差分を計算
		if (add <= kScoreThreshold) // 加算の値が閾値を超えたら
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

	// ゲージが取得出来たら
	if (auto gauge = m_pCoinGauge.lock())
	{
		// 敵をコインに変えている時間をゲージで表示する
		gauge->SetValue(GetChangeToCoinTimeRate());
	}

	// アイテムゲージがマックスになった瞬間を計測
	if (m_isMiniGame)
	{
		if (!m_isItemGaugeMax && GetMiniGameBalloonCounterRate() >= 1.0f)
		{
			Application::GetInstance().GetSoundManager()->Play("coinGaugeMaxSE", 1.0f, true);

			// ミニゲーム中にゲージが最大になったエフェクトを表示するのはアイテム生成時

			m_isItemGaugeMax = true;
		}
	}
	else
	{
		if (!m_isItemGaugeMax && GetBalloonCounterRate() >= 1.0f)
		{
			Application::GetInstance().GetSoundManager()->Play("coinGaugeMaxSE", 1.0f, true);
			if (auto manager = m_pEffectManager.lock())
			{
				manager->CreateEffekseerEffect(Types::EffectType::ChangeCoin, kCoinGaugeIconPos, false);
			}
			m_isItemGaugeMax = true;

		}
	}
	
	// アイテム化時間の時
	if (!m_isChangeToCoin && m_pEnemyManager->GetItemTimeRate() > 0.0f)
	{
		// ゲージがあればゲージを表示する
		if (auto gauge = m_pCoinGauge.lock())
		{
			gauge->SetActive(true);
		}
		else // なければ新しく作る
		{
			auto manager = m_pUIManager.lock();
			m_pCoinGauge = manager->CreateGauge(kCoinTimeGaugeSize, kCoinTimeGaugePos, kCoinTimeGaugeNum);
		}
		m_isChangeToCoin = true;
	}

	// 敵をコインに変える時間が終わったら
	if (m_isChangeToCoin && m_pEnemyManager->GetItemTimeRate() <= 0.0f)
	{
		// ゲージがあればゲージを非表示にする
		if (auto gauge = m_pCoinGauge.lock())
		{
			gauge->SetActive(false);
		}
		// 上書きしていたBGMを戻す
		Application::GetInstance().GetSoundManager()->EndTemporaryBGM(kChangeToCoinBGMFadeTime);
		m_isChangeToCoin = false;
	}

	if(IsGameOver()) // ゲームオーバー状態なら更新処理を行わない
	{
		printfDx(L"Game Over\n");
		return;
	}

	// クリア状態かミス状態の時はプレイヤー以外の更新処理を行わない
	if (IsClear() || m_pPlayer->IsMiss())
	{
		m_pPlayer->Update(input);
	}
	else
	{
		m_pPlayer->Update(input);
		m_pChestManager->Update(input);
		m_pItemManager->Update(input);
		m_pEnemyManager->Update(input);
	}
}

void GameManager::Draw() const
{
	// 仮の表示用
	//DrawFormatString(10, 100, 0xffffff, L"残機:%d", m_life);
	//DrawFormatString(10, 120, 0xffffff, L"メダルの所持数:%d", m_medalNum);
	//DrawFormatString(10, 140, 0xffffff, L"風船を取った数:%d", m_balloonNum);
	m_pEnemyManager->Draw();
}

void GameManager::PushActors(std::vector<std::weak_ptr<Actor>>& actors)
{
	actors.push_back(m_pPlayer);
	m_pChestManager->PushActors(actors);
	m_pItemManager->PushActors(actors);
	m_pEnemyManager->PushActors(actors);
}

void GameManager::AddScore(int score)
{
	m_currentScore += score;
}

void GameManager::ChangeEnemyToCoin()
{
	auto effManager = m_pEffectManager.lock();
	effManager->CreateEffekseerEffect(Types::EffectType::ChangeCoinStart, { Game::kScreenWidth / 2,Game::kScreenHeight / 2 }, false);
	
	// もともとコインに変える時間でなければ
	if (!m_isChangeToCoin)
	{
		// BGMを一時的に変更
		Application::GetInstance().GetSoundManager()->BeginTemporaryBGM("changeToCoinBGM", kChangeToCoinBGMFadeTime);
	}
	// 敵をアイテム化する通知を送る
	m_pEnemyManager->ChangeToItemAll();
}

bool GameManager::IsSkipCollision() const
{
	return IsClear() || m_pPlayer->IsMiss();
}

bool GameManager::IsClear() const
{
	// ゴールが開いていない場合判定しない
	if (!m_isOpenGoal)
	{
		return false;
	}

	auto pMap = m_pMap.lock();

	// ゴールの座標
	Position2 goalPos = pMap->GetGoalPosToMap();
	// ゴールの範囲を設定
	Rect2D goalRange = { goalPos, kGoalWidth, kGoalHeight };
	// プレイヤーの矩形を取得
	Rect2D playerRect = m_pPlayer->GetColRect();
	// 確実に当たっていない状況を判定する
	if (playerRect.GetLeft() > goalRange.GetRight())	return false;
	if (playerRect.GetTop() > goalRange.GetBottom())	return false;
	if (playerRect.GetRight() < goalRange.GetLeft())	return false;
	if (playerRect.GetBottom() < goalRange.GetTop())	return false;
	return true;
}

bool GameManager::IsGameOver() const
{
	// 残機が0以下で、プレイヤーのミス状態が終わればゲームオーバー
	return m_life <= 0 && !m_pPlayer->IsMiss();
}

void GameManager::OnItemCollected(const Types::ItemType& type, const Position2& pos)
{
	if (m_itemCollectFunc.contains(type))
	{
		m_itemCollectFunc[type](pos);
	}
	else
	{
		assert(false && L"アイテムを取った際の関数が見つかりませんでした");
	}
}

void GameManager::DropItem(const Position2& pos)
{
	m_pItemManager->SpawnItem(pos,Types::ItemType::UpgradeMedal);
	if(IsDropChangeToCoin()) // 敵をコインに変えるアイテムを落とすか判定
	{
		// アイテムを落としたときのSEを鳴らす
		Application::GetInstance().GetSoundManager()->Play("dropChangeToCoinSE", 1.0f, true);
		m_pItemManager->SpawnItem(pos, Types::ItemType::ChangeToCoin);
	}
}

void GameManager::DropItem(const Position2& pos, const Types::ItemType& type)
{
	m_pItemManager->SpawnItem(pos, type);
	// チュートリアルでない場合
	if (IsDropChangeToCoin() && !m_isTutorial) // 敵をコインに変えるアイテムを落とすか判定
	{
		// アイテムを落としたときのSEを鳴らす
		Application::GetInstance().GetSoundManager()->Play("dropChangeToCoinSE", 1.0f, true);
		m_isItemGaugeMax = false;
		m_pItemManager->SpawnItem(pos, Types::ItemType::ChangeToCoin);
	}
}

void GameManager::SpawnFloatingItem(const Position2& pos)
{
	// ゲージがMaxになった時のエフェクトを表示する
	if (auto manager = m_pEffectManager.lock())
	{
		manager->CreateEffekseerEffect(Types::EffectType::ChangeCoin, kCoinGaugeIconPos, false);
	}
	// アイテムを落としたときのSEを鳴らす
	Application::GetInstance().GetSoundManager()->Play("dropChangeToCoinSE", 1.0f, true);
	m_balloonCounter = 0;
	m_pItemManager->SpawnFloatingChangeToCoin(pos, m_pMap);
}

void GameManager::SpawnEnemy(const Position2& pos, int formNum)
{
	m_pEnemyManager->SpawnEnemy(pos, formNum);
}

void GameManager::PowerUpPlayer()
{
	// メダルを持っていれば強化を行う
	if (m_medalNum > 0)
	{
		if (m_pPlayer->PowerUp()) // プレイヤーの強化ができれば
		{
			m_medalNum--; // メダルを一つ減らす
			// プレイヤーを強化したときのSEを鳴らす
			Application::GetInstance().GetSoundManager()->Play("powerUpSE", 1.0f, true);
		}
		else // 強化が出来なければ
		{
			// 強化ができなかった時のSEを鳴らす
			Application::GetInstance().GetSoundManager()->Play("notPowerUpSE", 1.0f, true);
		}
	}
	else // メダルを持っていなければ
	{
		// 強化ができなかった時のSEを鳴らす
		Application::GetInstance().GetSoundManager()->Play("notPowerUpSE", 1.0f, true);
#ifdef _DEBUG
		printfDx(L"メダルがないので強化できない\n");
#endif
	}
}

void GameManager::ChangeFreezePlayer()
{
	m_pPlayer->FreezeChange();
}

void GameManager::OpenChestToPosition(int x, int y)
{
	m_pChestManager->OpenChestAtPosition(x,y);
}

bool GameManager::IsExceededPlayer(const Position2& area)
{
	return m_pPlayer->GetPos().x >= area.x;
}

bool GameManager::IsPowerUpPlayer()
{
	return m_pPlayer->IsPowerUp();
}

bool GameManager::IsGetBalloon(int balloonNum, bool isAll)
{
	if (isAll)
	{
		// 風船を取得した数分のカウンタから比較
		return m_balloonNum >= balloonNum;
	}
	else
	{
		// ミニゲーム内の風船を取る割合を設定
		m_miniGameBalloonCounterRate = static_cast<float>(m_balloonCounter) / balloonNum;
		// リセットありの風船のカウンタから比較
		return m_balloonCounter >= balloonNum;
	}
}

void GameManager::SetBarrierPlayer(const ActivePosition2& barrier)
{
	m_pPlayer->SetBarrier(barrier);
}

const size_t GameManager::GetActorNum() const
{
	// 各マネージャーの管理しているActorの数+プレイヤーの数を加算(+1)
	return kPlayerNum + m_pChestManager->GetChestNum() + m_pItemManager->GetItemNum() + m_pEnemyManager->GetEnemyNum();
}

const Position2& GameManager::GetPlayerPos() const
{
	return m_pPlayer->GetPos();
}

float GameManager::GetBalloonCounterRate() const
{
	return static_cast<float>(m_balloonCounter) / m_changeToCoinDropBalloonNum;
}

float GameManager::GetMiniGameBalloonCounterRate() const
{
	return m_miniGameBalloonCounterRate;
}

float GameManager::GetChangeToCoinTimeRate() const
{
	return m_pEnemyManager->GetItemTimeRate();
}

void GameManager::MissStart()
{
	// 残機を減らす
	m_life--;

	// ミスのテキストを出すためにlock
	auto manager = m_pUIManager.lock();

	// ミスのテキストのweak_ptrを取得
	auto weakMiss = manager->CreateFormatText(Types::FontType::Large, "", { Game::kScreenWidth / 2, Game::kScreenHeight / 2 });

	// lockしてミス時に表示するテキストの内容を設定
	auto miss = weakMiss.lock();
	miss->SetCenter();
	miss->SetProvider([this]() {
		return "Miss...";
		});
	miss->ShowTypewriter(kMissTextTypeFrame, false);
	miss->SetFadeOut(kMissTextWaitFrame, kMissTextFadeFrame);
}

void GameManager::NotifyItemEvent(Types::ItemType type)
{
	m_itemPickEvent.push_back({ type, m_frameCount });
}

bool GameManager::IsItemPicked(Types::ItemType itemType)
{
	for (auto it = m_itemPickEvent.begin(); it != m_itemPickEvent.end(); it++)
	{
		if (it->type == itemType)
		{
			m_itemPickEvent.erase(it);
			return true;
		}
	}
	return false;
}

void GameManager::RequestCreateEffect(Types::EffectType effectType, const Position2& pos, bool isUseCamera)
{
	if (auto manager = m_pEffectManager.lock())
	{
		manager->CreateEffekseerEffect(effectType, pos, isUseCamera);
	}
}

std::weak_ptr<EffekseerEffect> GameManager::RequestCreateEffect(Types::EffectType effectType, const Position2& pos, std::weak_ptr<Actor> provider, bool isUseCamera)
{
	if (auto manager = m_pEffectManager.lock())
	{
		return manager->CreateEffekseerEffectWithProvider(effectType, pos, provider, isUseCamera);
	}
	return {};
}

void GameManager::CreateReadyGoText()
{
	auto manager = m_pUIManager.lock();


	auto weakStart = manager->CreateFormatText(Types::FontType::Large, "", { Game::kScreenWidth / 2, Game::kScreenHeight / 2 });
	
	auto start = weakStart.lock();
	start->SetCenter();
	start->SetProvider([this]() {
		return "Ready...Go!";
		});
	start->ShowTypewriter(kReadyGoTextTypeFrame, false);
	start->SetFadeOut(kReadyGoTextWaitFrame, kReadyGoTextFadeFrame);
}

void GameManager::SetLastStageMode()
{
	// 敵をコインに変えるアイテムを生成するまでの値が多くなる
	m_changeToCoinDropBalloonNum = kLastStageBalloonForChangeToCoin;
	// 敵の生成スピードが速くなる
	m_pEnemyManager->SpawnFast();
}

bool GameManager::IsDropChangeToCoin()
{
	if(m_balloonCounter >= m_changeToCoinDropBalloonNum) // 風船を指定数取ったら
	{
		m_balloonCounter = 0; // カウンターをリセット
		return true;
	}
	return false;
}
