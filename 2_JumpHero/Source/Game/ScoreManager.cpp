#include "ScoreManager.h"
#include "../Utility/StringFunction.h"
#include "DxLib.h"

namespace
{
	constexpr int kSignatureNum = 4;

	constexpr char kScoreFileDataName[] = "scoreData.scdt";

	/// <summary>
	/// スコアデータを管理する用の構造体
	/// </summary>
	struct ScoreHeader
	{
		char signature[kSignatureNum];
		float version;
		int dataNum;
	};
}

void ScoreManager::Load()
{
	auto fileName = StringFunction::WStringFromString(kScoreFileDataName);
	int handle = FileRead_open(fileName.c_str());
	if (handle == -1) return; // ファイルがない場合以下の処理を行わない
	ScoreHeader header = {};
	FileRead_read(&header, sizeof(header), handle);

	if (header.signature[0] != 's' ||
		header.signature[1] != 'c' ||
		header.signature[2] != 'd' ||
		header.signature[3] != 't')
	{
		FileRead_close(handle);
		return; // データが正しくない場合この処理をする
	}

	FileRead_read(&m_data, sizeof(m_data), handle);

	FileRead_close(handle);

}

void ScoreManager::UpdateHighScore(int stageNo, int score)
{
	int* target = nullptr;

	switch (stageNo)
	{
	case 0:
		target = &m_data.tutorialScore;
		break;
	case 1:
		target = &m_data.stage1Score;
		break;
	case 2:
		target = &m_data.stage2Score;
		break;
	case 3:
		target = &m_data.stage3Score;
		break;
	default:
		return;
	}

	if (score > *target)
	{
		*target = score;
		Save();
	}
}

int ScoreManager::GetHighScore(int stageNo)
{
	switch (stageNo)
	{
	case 0:
		return m_data.tutorialScore;
	case 1:
		return m_data.stage1Score;
	case 2:
		return m_data.stage2Score;
	case 3:
		return m_data.stage3Score;
	default:
		break;
	}
	return 0;
}

void ScoreManager::Save()
{
	FILE* fp = nullptr;

	// バイナリデータを保存する
	auto err = fopen_s(&fp, kScoreFileDataName, "wb");

	if (fp == nullptr)
	{
		return;
	}

	ScoreHeader header = {};

	header.signature[0] = 's';
	header.signature[1] = 'c';
	header.signature[2] = 'd';
	header.signature[3] = 't';

	header.version = 1.0f;
	header.dataNum = sizeof(m_data);

	// ヘッダへの書き込み
	fwrite(&header, sizeof(header), 1, fp);

	// 実際のデータへ書き込み
	fwrite(&m_data, sizeof(m_data), 1, fp);

	// ファイルを閉じる
	fclose(fp);
}
