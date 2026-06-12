#include "GameManager.h"

namespace
{
	// ステージを進める数
	constexpr int kStageAdvanceNum = 1;

	// 倍率に応じたボーナススコアをもらえる最大時間
	constexpr float kBonusMaxTimes[] =
	{
		30.0f,
		35.0f,
		40.0f
	};
}

GameManager::GameManager() :
	m_currentStage(StageType::Stage1),
	m_currentClearTime(0),
	m_currentStageScore(0),
	m_totalScore(0)
{
}

void GameManager::Init()
{
	// ステージのデータを初期化
	m_currentStage = StageType::Stage1;	// ステージを最初のステージにする
	m_currentClearTime = 0;				// 現在のクリアタイムを0にする
	m_currentStageScore = 0;			// 現在のステージのスコアを0にする
	m_totalScore = 0;					// ゲーム全体のスコアを0にする

	// 全ステージ分のメモリを確保しておく
	m_stageDatas.resize(static_cast<int>(StageType::Max));
}

const float GameManager::GetBonusMaxTime() const
{
	// クリアしたステージを確認(現在のステージ番号-1)
	int clearStageIdx = static_cast<int>(m_currentStage) - kStageAdvanceNum;

	// ステージ番号がステージの数を超えていたら
	if (clearStageIdx > static_cast<int>(StageType::Stage3))
	{
		// 0を返す
		return 0.0f;
	}
	// ステージ番号が存在しないものなら
	else if (clearStageIdx < static_cast<int>(StageType::Stage1))
	{
		// 0を返す
		return 0.0f;
	}

	// クリアしたステージに応じたボーナスの最大時間を返す
	return kBonusMaxTimes[clearStageIdx];
}

GameManager::StageData GameManager::GetStageData(StageType stage)
{
	return m_stageDatas[static_cast<int>(stage)];
}

void GameManager::AddScore(int score)
{
	// 全体のスコアを加算
	m_totalScore += score;
	// 現在のステージのスコアを加算
	m_currentStageScore += score;
}

void GameManager::OnStageClear(int clearTime)
{
	// クリアタイムを受け取って更新
	m_currentClearTime = clearTime;

	// 現在のステージの番号を取得
	int idx = static_cast<int>(m_currentStage);

	// 現在のスコアがハイスコアより高い場合
	if (m_currentStageScore > m_stageDatas[idx].highScore)
	{
		// ハイスコアを更新
		m_stageDatas[idx].highScore = m_currentStageScore;
	}

	// 現在のクリアタイムがベストタイムより速い場合
	if (m_currentClearTime < m_stageDatas[idx].bestTime)
	{
		// ベストタイムを更新
		m_stageDatas[idx].bestTime = m_currentClearTime;
	}

	// 次のシーンを確認(現在のステージ番号+1)
	int nextIdx = idx + kStageAdvanceNum;

	// 次のシーンがある場合
	if (nextIdx < static_cast<int>(StageType::Max))
	{
		// 現在のステージを進める
		m_currentStage = static_cast<StageType>(nextIdx);
	}
	else
	{
		// 次のシーンが存在しないため更新しない
	}
}

void GameManager::ResetCurrentStageData()
{
	m_currentClearTime = 0;		// 現在のクリアタイムを0にする
	m_currentStageScore = 0;	// 現在のステージのスコアを0にする
}
