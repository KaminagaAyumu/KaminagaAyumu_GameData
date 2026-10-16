#include "Map.h"
#include "Camera.h"
#include "Game.h"
#include <fstream>
#include "DxLib.h"
#include <algorithm>
#include <cassert>

namespace
{
	constexpr int kTitleStageNo = -1; // タイトルのステージの番号
	constexpr int kTutorialStageNo = 0; // チュートリアルのステージの番号

	constexpr int kIdentifierSize = 4; // ファイル識別子の文字数

	constexpr int kMapData = 0; // マップデータのレイヤー番号
	constexpr int kPositioningData = 1; // 配置データのレイヤー番号
	constexpr int kEventData = 2; // イベントデータのレイヤー番号

	// イベント発火フラグの場所を取得するための番号(修正する可能性あり)
	constexpr int kFirstTriggerNum = 1;
	constexpr int kLastTriggerNum = 6;

	// マップのチップを置く数
	constexpr int kChipNumX = 97;
	constexpr int kChipNumY = 16;

	constexpr int kChipSize = 32; // マップ1つの大きさ
	constexpr float kChipScale = 1.40625f; // マップの拡大率

	// マップチップ
	constexpr int kSpaceChipNo = 0; // マップチップの透明部分
	constexpr int kChestChipNo = 106; // マップチップの宝箱部分
	constexpr int kHiddenChestChipNo = 107; // マップチップの隠し宝箱部分

	constexpr int kStartChipNoUp = 188; // マップチップのスタート地点上部
	constexpr int kStartChipNoDown = 208;// マップチップのスタート地点下部

	constexpr int kGoalChipNoUp = 191; // マップチップのゴール地点上部
	constexpr int kGoalChipNoDown = 211;// マップチップのゴール地点下部

	// 描画の際のマップチップ番号
	constexpr int kGoalOpenChipNoUp = 192;
	constexpr int kGoalOpenChipNoDown = 212;

	constexpr int kGoalCloseChipNoUp = 190;
	constexpr int kGoalCloseChipNoDown = 210;

	constexpr int kThroughChipNoLeft = 264;
	constexpr int kThroughChipNoCenter = 265;
	constexpr int kThroughChipNoRight = 266;

	constexpr int kSignboardChipNo = 291; // 右を指す看板のマップチップ(当たり判定を行わない)

	constexpr float kRetRectMargin = 0.5f;

	constexpr float kMoveRangeMargin = 0.1f; // マップとの位置の補正用

	constexpr float kLargeValue = 100000.0f; // 大きな値(初期化用)

	constexpr int kFilePathSize = 64; // ファイルのパスの最大サイズ
	constexpr int kMapDataBitCount = 16; // マップデータのファイルのビットカウント

	// FMFファイルのデータ構造
	// 
	//	+ 0	identifier[U4]	// ファイル識別子 'FMF_' (0x5F464D46) 4バイト
	//	+ 4	size[U4]	// ヘッダを除いたデータサイズ 4バイト
	//	+ 8	mapWidth[U4]	// マップの横幅 4バイト
	//	+ 12	mapHeight[U4]	// マップの縦幅 4バイト
	//	+ 16	chipHeight[U1]	// パーツの横幅 1バイト
	//	+ 17	chipHeight[U1]	// パーツの縦幅 1バイト
	//	+ 18	layerCount[U1]	// レイヤー数 1バイト
	//	+ 19	bitCount[U1]	// レイヤーデータのビットカウント(8/16) 1バイト
	// 計20バイト

	struct DataSetting // マップのデータを読み込む際に使う
	{
		uint8_t chipW; // マップチップの横幅(今回は使わない)
		uint8_t chipH; // マップチップの縦幅(今回は使わない)
		uint8_t layerCount; // レイヤーの数
		uint8_t bitCount; // 8ビットか16ビットか(今回は使わない)
	};

	struct DataHeader // マップのデータを読み込む構造体
	{
		char identifier[4]; // ファイル識別子 FMF_
		uint32_t size; // ヘッダを除いたデータサイズ
		uint32_t width; // マップの幅
		uint32_t height; // マップの高さ
		DataSetting setting; // パーツデータやレイヤー数、ビットカウントを取得
	};

}

Map::Map(int stageNo, bool isMiniGame) :
	m_chipData{},
	m_layerMapData{},
	m_graphChipNumX(0),
	m_graphChipNumY(0)
{
	m_mapHandle = LoadGraph(L"data/gameData.png");
	//m_mapHandle = LoadGraph(L"data/mapChip_default.png");

	// マップのデータと画像の切り取り位置を合わせるための処理
	int graphW = 0;
	int graphH = 0;
	GetGraphSize(m_mapHandle, &graphW, &graphH); // 画像のサイズを取得

	// マップの切り取り部分の数を取得
	m_graphChipNumX = graphW / kChipSize;
	m_graphChipNumY = graphH / kChipSize;

	// マップの初期データをロード
	//LoadMapdata("data/map_new.csv");
	if (!LoadStageData(stageNo, isMiniGame))
	{
		assert(false && "ステージデータの読み込みに失敗しました");
	}
}

Map::~Map()
{
	DeleteGraph(m_mapHandle);
}

void Map::Init()
{

}

void Map::Update()
{

}

void Map::Draw(std::weak_ptr<Camera> camera)
{
	auto pCamera = camera.lock();
	// 画面のスクロール量
	float scrollX = pCamera->scroll.x;
	float scrollY = pCamera->scroll.y;

	// 1マスの描画サイズ
	float tileSize = kChipSize * kChipScale;

	// 表示範囲を計算
	int startX = static_cast<int>(scrollX / tileSize);
	int startY = static_cast<int>(scrollY / tileSize);
	int endX = static_cast<int>((scrollX + Game::kScreenWidth) / tileSize) + 1;
	int endY = static_cast<int>((scrollY + Game::kScreenHeight) / tileSize) + 1;

	// 画面左端にいる時
	if (startX < 0) startX = 0;
	if (startY < 0) startY = 0;
	// 画面右端にいる時
	if (endX > m_width) endX = m_width;
	if (endY > m_height) endY = m_height;

	for (int y = startY; y < endY; y++)
	{
		for (int x = startX; x < endX; x++)
		{
			// 表示したい画像の範囲
			const float dstLeft = x * tileSize - scrollX; // 左端
			const float dstTop = y * tileSize - scrollY; // 上端
			const float dstRight = dstLeft + tileSize; // 右端
			const float dstBottom = dstTop + tileSize; // 下端

			//const int chipNo = m_chipData[y * m_width + x];
			const int chipNo = m_layerMapData[kMapData][y * m_width + x]; // レイヤーの番号
			const int srcX = kChipSize * (chipNo % m_graphChipNumX); // どこの行かを座標に変換
			const int srcY = kChipSize * (chipNo / m_graphChipNumX); // どこの列かを座標に変換

			DrawRectExtendGraph(static_cast<int>(dstLeft), static_cast<int>(dstTop),
				static_cast<int>(dstRight), static_cast<int>(dstBottom),
				srcX, srcY,
				kChipSize, kChipSize, m_mapHandle, true);
		}
	}
}

bool Map::IsCollision(const Rect2D& rect, Rect2D& mapRect)
{
	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			//int chipNo = m_chipData[y * m_width + x];
			int chipNo = m_layerMapData[kMapData][y * m_width + x];
			if (chipNo == kSpaceChipNo)
			{
				continue; // マップチップの透明部分は当たり判定をしないようにする
			}

			// 1マスの描画サイズ
			float tileSize = kChipSize * kChipScale;

			// 当たり判定を表示する座標
			// xとyは当たり判定の左端なので中央に補正
			float posX = x * tileSize + tileSize * 0.5f;
			float posY = y * tileSize + tileSize * 0.5f;

			Rect2D chipRect = { {posX,posY},tileSize,tileSize };
			if (chipRect.GetLeft() > rect.GetRight())
			{
				continue;
			}
			if (chipRect.GetRight() < rect.GetLeft())
			{
				continue;
			}
			if (chipRect.GetTop() > rect.GetBottom())
			{
				continue;
			}
			if (chipRect.GetBottom() < rect.GetTop())
			{
				continue;
			}

			// マップの矩形を返す
			mapRect = chipRect;
			return true;
		}
	}
	return false;
}

Rect2D Map::GetCanMoveRange(const Rect2D& rect)
{
	// 1マスの描画サイズ
	const float tileSize = kChipSize * kChipScale;

	// 引数の矩形の上下左右の座標
	const float rectLeft = rect.pos.x - rect.width * 0.5f; // 矩形の左端
	const float rectRight = rect.pos.x + rect.width * 0.5f; // 矩形の右端
	const float rectTop = rect.pos.y - rect.height * 0.5f; // 矩形の上端
	const float rectBottom = rect.pos.y + rect.height * 0.5f; // 矩形の下端

	// 引数の矩形の座標をマップの座標基準に変換
	// (マップの座標基準でどこにいるか)
	int topTileY = std::clamp(WorldPosToMapPos(rectTop, tileSize), 0, m_height - 1);
	int bottomTileY = std::clamp(WorldPosToMapPos(rectBottom, tileSize), 0, m_height - 1);
	int leftTileX = std::clamp(WorldPosToMapPos(rectLeft, tileSize), 0, m_width - 1);
	int rightTileX = std::clamp(WorldPosToMapPos(rectRight, tileSize), 0, m_width - 1);

	// 移動可能範囲の矩形の初期化
	// 初期状態ではマップ全体とする
	float retRectLeft = 0.0f; // マップの左端
	float retRectRight = m_width * tileSize; // マップの右端
	float retRectTop = 0.0f; // マップの上端
	float retRectBottom = m_height * tileSize; // マップの下端

	// 左側を見る
	// Y座標は矩形が当たる範囲分ループ
	for (int y = topTileY; y <= bottomTileY; ++y)
	{
		// X座標は矩形の左端からマップの左端に到達するまでループ
		for (int x = leftTileX; x >= 0; --x)
		{
			int chipNo = m_layerMapData[kMapData][y * m_width + x]; // マップチップ番号を確かめる
			if (IsNomalTile(chipNo)) // マップチップが通常ならば
			{
				// マップチップの座標は左端基準なので
				// 見つかったマップチップから1つ右側の座標が右端となる
				float tileRight = (x + 1) * tileSize;
				// 返り値にする矩形の左端座標を設定
				// 左端座標がすでに設定されているものより大きかったら更新、
				// それより小さいなら更新しない
				retRectLeft = max(retRectLeft, tileRight);
				break;
			}
		}
	}

	// 右側を見る
	// Y座標は矩形が当たる範囲分ループ
	for (int y = topTileY; y <= bottomTileY; ++y)
	{
		// X座標は矩形の左端からマップの左端に到達するまでループ
		for (int x = rightTileX; x < m_width; ++x)
		{
			int chipNo = m_layerMapData[kMapData][y * m_width + x]; // マップチップ番号を確かめる
			if (IsNomalTile(chipNo)) // マップチップが通常ならば
			{
				// マップチップの座標は左端基準なので
				// 見つかったマップチップの座標が左端となる
				float tileLeft = x * tileSize;
				// 返り値にする矩形の右端座標を設定
				// 右端座標がすでに設定されているものより小さかったら更新、
				// それより大きいなら更新しない
				retRectRight = min(retRectRight, tileLeft);
				break;
			}
		}
	}

	// 上側を見る
	// X座標は矩形が当たる範囲分ループ
	for (int x = leftTileX; x <= rightTileX; ++x)
	{
		// Y座標は矩形の上端からマップの上端に到達するまでループ
		for (int y = topTileY; y >= 0; --y)
		{
			int chipNo = m_layerMapData[kMapData][y * m_width + x]; // マップチップ番号を確かめる
			if (IsNomalTile(chipNo)) // マップチップが通常ならば
			{
				// マップチップの座標は左端基準なので
				// 見つかったマップチップの座標から1つ下側の座標が下端となる
				float tileBottom = (y + 1) * tileSize;
				// 返り値にする矩形の上端座標を設定
				// 上端座標がすでに設定されているものより大きかったら更新、
				// それより小さいなら更新しない
				retRectTop = max(retRectTop, tileBottom);
				break;
			}
		}
	}

	// 下側を見る
	// X座標は矩形が当たる範囲分ループ
	for (int x = leftTileX; x <= rightTileX; ++x)
	{
		// Y座標は矩形の上端からマップの上端に到達するまでループ
		for (int y = bottomTileY; y < m_height; ++y)
		{
			int chipNo = m_layerMapData[kMapData][y * m_width + x]; // マップチップ番号を確かめる
			if (IsNomalTile(chipNo)) // マップチップが通常ならば
			{
				// マップチップの座標は左端基準なので
				// 見つかったマップチップの座標が上端となる
				float tileTop = y * tileSize;
				// 返り値にする矩形の下端座標を設定
				// 下端座標がすでに設定されているものより小さかったら更新、
				// それより大きいなら更新しない
				retRectBottom = min(retRectBottom, tileTop);
				break;
			}
		}
	}

	retRectLeft += kMoveRangeMargin;
	retRectRight -= kMoveRangeMargin;
	retRectTop += kMoveRangeMargin;
	retRectBottom -= kMoveRangeMargin;

	// 上下左右の座標が逆になっている場合は
	// 返す矩形の範囲をリセットする(1.0の大きさの矩形にする)
	// 左の座標が右の座標よりも大きくなっている場合
	if (retRectLeft > retRectRight) {
		// 左右の距離の中心を取得
		float centerPos = (retRectLeft + retRectRight) * 0.5f;
		retRectLeft = centerPos - kRetRectMargin; // 中心から0.5引いた座標
		retRectRight = centerPos + kRetRectMargin; // 中心から0.5足した座標
	}
	// 上の座標が下の座標よりも大きくなっている場合
	if (retRectTop > retRectBottom) {
		// 上下の距離の中心を取得
		float centerPos = (retRectTop + retRectBottom) * 0.5f;
		retRectTop = centerPos - kRetRectMargin; // 中心から0.5引いた座標
		retRectBottom = centerPos + kRetRectMargin; // 中心から0.5足した座標
	}

	// 返す矩形の中心座標を設定
	Position2 newPos = {
		(retRectLeft + retRectRight) * 0.5f, // 左右の座標を足して2で割ると中央になる
		(retRectTop + retRectBottom) * 0.5f  // 上下の座標を足して2で割ると中央になる
	};

	float newWidth = retRectRight - retRectLeft; // 右端座標から左端座標を引いた値が幅となる
	float newHeight = retRectBottom - retRectTop; // 下端座標から上端座標を引いた値が高さとなる

	// 設定した矩形を返す
	return Rect2D(newPos, newWidth, newHeight);
}

bool Map::IsNomalTile(int chipNo)
{
	// 特殊な条件でなければ通常のタイルとする
	return !IsSpaceTile(chipNo) && !IsOnlyTopTile(chipNo);
}

bool Map::IsSpaceTile(int chipNo)
{
	// 透明部分のマップチップかどうかを返す
	return chipNo == kSpaceChipNo || chipNo == kSignboardChipNo;
}

bool Map::IsOnlyTopTile(int chipNo)
{
	// 通過可能なマップチップかどうかを返す
	return chipNo == kThroughChipNoLeft || chipNo == kThroughChipNoCenter || chipNo == kThroughChipNoRight;
}

bool Map::IsChestTile(int chipNo)
{
	return chipNo == kChestChipNo || chipNo == kHiddenChestChipNo;
}

bool Map::IsEventFlagTile(int chipNo)
{
	return chipNo >= kFirstTriggerNum && chipNo <= kLastTriggerNum;
}

float Map::GetTileSize() const
{
	return kChipSize * kChipScale;
}

Size Map::GetMapSize() const
{
	return { static_cast<int>(m_width * kChipSize * kChipScale),static_cast<int>(m_height * kChipSize * kChipScale) };
}

int Map::GetMapChipNum(int x, int y)
{
	return m_layerMapData[kMapData][y * m_width + x];
}

int Map::GetPositioningData(int x, int y)
{
	return m_layerMapData[kPositioningData][y * m_width + x];
}

int Map::GetEventData(int x, int y)
{
	return m_layerMapData[kEventData][y * m_width + x];
}

Position2 Map::GetStartPosToMap()
{
	Position2 startPos = Position2();
	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			int chipNo = m_layerMapData[kEventData][y * m_width + x];
			if (chipNo == kStartChipNoUp)
			{
				startPos.x = x * kChipSize * kChipScale - (kChipSize * kChipScale) / 2.0f;
				startPos.y = y * kChipSize * kChipScale + (kChipSize * kChipScale);
				return startPos;
			}
		}
	}
	// ここまで来た場合、スタート地点が見つからないのでエラーとする
	assert(false && "スタート地点のマップチップが見つかりません");
	return Position2();
}

Position2 Map::GetGoalPosToMap()
{
	Position2 goalPos = Position2();
	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			int chipNo = m_layerMapData[kEventData][y * m_width + x];
			if (chipNo == kGoalChipNoUp)
			{
				// X座標はマップチップの左端に
				goalPos.x = x * kChipSize * kChipScale;
				// Y座標はマップチップの下端に
				goalPos.y = y * kChipSize * kChipScale + (kChipSize * kChipScale);
				return goalPos;
			}
		}
	}
	// ここまで来た場合、ゴールが見つからないのでエラーとする
	assert(false && "ゴール地点のマップチップが見つかりません");
	return Position2();
}

void Map::ResetChestData(int x, int y)
{
	m_layerMapData[kMapData][y * m_width + x] = kSpaceChipNo;
	m_layerMapData[kPositioningData][y * m_width + x] = kSpaceChipNo;
}

void Map::SetMapChip(int x, int y, int value)
{
	//m_chipData[y * m_width + x] = value;
	m_layerMapData[kMapData][y * m_width + x] = value;
}

void Map::SetGoalMapChipOpen()
{
	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			int chipNo = m_layerMapData[kEventData][y * m_width + x];
			if (chipNo == kGoalChipNoUp)
			{
				m_layerMapData[kMapData][y * m_width + x] = kGoalOpenChipNoUp;
			}
			if (chipNo == kGoalChipNoDown)
			{
				m_layerMapData[kMapData][y * m_width + x] = kGoalOpenChipNoDown;
			}
		}
	}
}

void Map::LoadMapdata(const std::string& fileName)
{
	std::ifstream file(fileName);
	std::string line;
	std::vector<std::vector<int>> temp; // 格納用の二次元配列

	while (std::getline(file, line))
	{

		std::istringstream stream(line);
		std::string field;
		std::vector<int> row; // マップの行を格納する用の配列

		while (getline(stream, field, ','))
		{
			row.push_back(std::stoi(field)); // 行に要素を格納
		}
		temp.push_back(row); // 列に要素を格納
	}

	m_height = static_cast<int>(temp.size()); // 列の要素数を取得
	m_width = static_cast<int>(temp[0].size()); // 行の要素数を取得
	m_chipData.resize(m_width * m_height); // 実際に格納するメンバ変数のサイズを取得

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			m_chipData[y * m_width + x] = temp[y][x];
		}
	}
}

bool Map::LoadStageData(int stageNo, bool isMiniGame)
{
	// 読み込むステージデータのパスを取得するための変数
	wchar_t filePath[kFilePathSize];
	// ステージの番号に対応したパスを取得する
	if (isMiniGame) // ミニゲームシーンのマップをロードする
	{
		if (stageNo == 0)
		{
			// チュートリアルの場合(未実装)
			std::swprintf(filePath, kFilePathSize, L"data/minigame1.fmf");
		}
		else
		{
			// 通常のミニゲームの場合
			std::swprintf(filePath, kFilePathSize, L"data/minigame%d.fmf", stageNo);
		}
	}
	else // 通常のゲームシーンのマップをロードする
	{
		if (stageNo == kTitleStageNo)
		{
			// タイトルの場合
			std::swprintf(filePath, kFilePathSize, L"data/titleData.fmf");
		}
		else if (stageNo == kTutorialStageNo)
		{
			// チュートリアルの場合
			std::swprintf(filePath, kFilePathSize, L"data/tutorialData.fmf");
		}
		else
		{
			// 通常のステージの場合
			std::swprintf(filePath, kFilePathSize, L"data/stage%dData.fmf", stageNo);
		}
	}

	// パスに対応したステージデータのファイルを開く
	const int handle = FileRead_open(filePath);
	if (handle <= 0) // ファイルが読み込めていなかった場合
	{
		return false; // ロード失敗
	}

	// ヘッダを読み込む
	DataHeader header;

	/////////////////////////////////////////////////////////////////////////////////////////
	//ファイルの読み込み
	/////////////////////////////////////////////////////////////////////////////////////////

	// 読み込んだファイルのサイズがヘッダのサイズと一致していなければ
	if (FileRead_read(&header, sizeof(header), handle) != static_cast<int>(sizeof(header)))
	{
		FileRead_close(handle); // ファイルを閉じる
		return false; // ロード失敗
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	//ファイルのチェック
	/////////////////////////////////////////////////////////////////////////////////////////

	// ファイル識別子がFMF_でなければロード失敗とする
	if (std::memcmp(header.identifier, "FMF_", kIdentifierSize) != 0)
	{
		FileRead_close(handle); // ファイルを閉じる
		return false; // ロード失敗
	}

	// マップデータを取得
	const uint8_t layerCount = header.setting.layerCount;
	const uint8_t bitCount = header.setting.bitCount;
	const uint32_t headerWidth = header.width;
	const uint32_t headerHeight = header.height;

	// 16ビットではない、もしくはマップデータが入っていなければ
	if (bitCount != kMapDataBitCount || layerCount == 0 || headerWidth == 0 || headerHeight == 0)
	{
		FileRead_close(handle); // ファイルを閉じる
		return false; // ロード失敗
	}

	// レイヤーのバイトサイズ類
	const size_t elemSize = sizeof(uint16_t); // 16ビットのバイトサイズ(2バイト)
	const size_t layerSize = static_cast<size_t>(headerWidth * headerHeight * elemSize); // 1つのレイヤーのバイト数
	const size_t totalSize = layerSize * layerCount; // すべてのレイヤーのバイトサイズ

	// ヘッダのサイズがレイヤー全体のバイト数と違った場合
	if (header.size != totalSize)
	{
		FileRead_close(handle); // ファイルを閉じる
		return false; // ロード失敗
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	//データの読み込み
	/////////////////////////////////////////////////////////////////////////////////////////

	// 行の読み込み
	std::vector<uint8_t> rawData(totalSize);
	if (FileRead_read(rawData.data(), static_cast<int>(totalSize), handle) != static_cast<int>(totalSize))
	{
		FileRead_close(handle);
		return false;
	}

	// 読み込みが終わったのでファイルを閉じる
	FileRead_close(handle);

	/////////////////////////////////////////////////////////////////////////////////////////
	//データをクラスのメンバに代入
	/////////////////////////////////////////////////////////////////////////////////////////

	// マップ情報を代入
	m_width = headerWidth;
	m_height = headerHeight;
	m_layerCount = layerCount;

	// マップのデータを代入
	m_layerMapData.clear(); // 初期化
	m_layerMapData.resize(layerCount); // レイヤーの数分配列を確保
	for (auto& layer : m_layerMapData) // レイヤーごとに配列をループしている
	{
		// レイヤーごとにマップのデータを確保
		layer.resize(static_cast<size_t>(headerWidth) * headerHeight);
	}

	// レイヤーの数分ループ
	for (uint8_t layer = 0; layer < layerCount; layer++)
	{
		// 8ビットの要素
		const uint8_t* begin = rawData.data() + static_cast<size_t>(layer * layerSize);
		// 8ビットの要素を16ビットに変換
		const uint16_t* data = reinterpret_cast<const uint16_t*>(begin);
		// データを上から格納
		for (size_t i = 0; i < headerWidth * headerHeight; i++)
		{
			m_layerMapData[layer][i] = data[i];
		}
	}
	// データの読み込みに成功したらtrueを返す
	return true;
}

int Map::WorldPosToMapPos(float pos, float tileSize)
{
	// 座標が負の値の場合
	if (pos < 0)
	{
		pos = 0; // 座標を0とする(マップの座標は0以下にならないため)
	}
	return static_cast<int>(std::floor(pos / tileSize));
}
