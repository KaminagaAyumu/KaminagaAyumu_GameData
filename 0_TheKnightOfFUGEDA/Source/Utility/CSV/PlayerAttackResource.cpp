#include "PlayerAttackResource.h"
#include <cassert>

namespace
{
	// CSVの列番号
	constexpr int kIdColumn = 0;	// 攻撃ID
	constexpr int kStartFrameColumn = 1;	// 発生フレーム
	constexpr int kActiveFrameColumn = 2;	// 持続フレーム
	constexpr int kComboFrameColumn = 3;	// コンボが繋がるフレーム
	constexpr int kAnimChangeFrameColumn = 4;	// 次のアニメーションに進めるフレーム
	constexpr int kCancelFrameColumn = 5;	// キャンセル可能なフレーム
	constexpr int kInitSpeedColumn = 6;	// 攻撃の初速度
	constexpr int kMoveEndFrameColumn = 7;	// 移動しなくなるフレーム数
	constexpr int kNextIdColumn = 8;	// 次の攻撃のID
	constexpr int kNextAnimColumn = 9;	// 次のアニメーションの名前
}

PlayerAttackResource::PlayerAttackResource()
{
}

PlayerAttackResource::~PlayerAttackResource()
{
}

void PlayerAttackResource::ConvertAttackData()
{
	// データの数を取得する
	size_t size = GetDataCount();

	for (size_t i = 0; i < size; ++i)
	{
		// データを読み込む
		int id = Read<int>(static_cast<int>(i), kIdColumn);
		int startFrame = Read<int>(static_cast<int>(i), kStartFrameColumn);
		int activeFrame = Read<int>(static_cast<int>(i), kActiveFrameColumn);
		int comboFrame = Read<int>(static_cast<int>(i), kComboFrameColumn);
		int animChangeFrame = Read<int>(static_cast<int>(i), kAnimChangeFrameColumn);
		int cancelFrame = Read<int>(static_cast<int>(i), kCancelFrameColumn);
		float initSpeed = Read<float>(static_cast<int>(i), kInitSpeedColumn);
		int moveEndFrame = Read<int>(static_cast<int>(i), kMoveEndFrameColumn);
		int nextID = Read<int>(static_cast<int>(i), kNextIdColumn);
		std::wstring nextAnim = Read<std::wstring>(static_cast<int>(i), kNextAnimColumn);

		// 攻撃データを作成
		std::shared_ptr<AttackData> data = std::make_shared<AttackData>();
		// データをセットする
		data->SetData(static_cast<AttackData::AttackID>(id), startFrame, activeFrame, comboFrame, animChangeFrame,cancelFrame, initSpeed, moveEndFrame, static_cast<AttackData::AttackID>(nextID), nextAnim);
		m_pAttackDatas[static_cast<AttackData::AttackID>(id)] = data;
	}

	// データを開放する
	ReleaseRawData();
}

AttackData* PlayerAttackResource::GetAttackData(AttackData::AttackID id)
{
	auto it = m_pAttackDatas.find(id);
	if (it != m_pAttackDatas.end())
	{
		return it->second.get();
	}
	else
	{
		assert(false && "PlayerAttackResource : IDに対応する攻撃データが見つかりませんでした");
	}
	return nullptr;
}
