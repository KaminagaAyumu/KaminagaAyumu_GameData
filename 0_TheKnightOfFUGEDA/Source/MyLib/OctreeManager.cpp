#include "OctreeManager.h"
#include "MyMath.h"
#include "OctreeCell.h"

namespace
{
	// 1つのセルが持つ子セルの数
	constexpr uint32_t kChildCellNum = 8;
	// モートン番号で1階層を表すのに使うビット数(x,y,zの3軸分)
	constexpr uint32_t kMortonBitsPerLevel = 3;
}

MyLib::OctreeManager::OctreeManager() :
	m_level(0),
	m_cellCount(0)
{
}

bool MyLib::OctreeManager::Init(uint32_t level, const BoundingBox& worldAABB)
{
	m_level = level;
	m_worldMin = worldAABB.min;

	// 最深レベルでの1セル当たりのサイズを計算
	uint32_t divNum = 1u << level;
	Vector3 worldSize = worldAABB.max - worldAABB.min;
	m_cellSize.x = worldSize.x / static_cast<float>(divNum);
	m_cellSize.y = worldSize.y / static_cast<float>(divNum);
	m_cellSize.z = worldSize.z / static_cast<float>(divNum);

	// セルの配列を確保
	m_cellCount = GetTotalCellCount(level);

	m_pCells.assign(m_cellCount, nullptr);

	return true;
}

void MyLib::OctreeManager::RemoveAll()
{
	for (auto& cell : m_pCells)
	{
		if (!cell) continue;

		// セル内の全OFTを走査してリンクをクリア
		std::shared_ptr<MyLib::ObjectForTree> pOFT = cell->GetLatestOFT().lock();
		while (pOFT)
		{
			std::shared_ptr<MyLib::ObjectForTree> next = pOFT->GetNext().lock();
			pOFT->SetPrev({});
			pOFT->SetNext({});
			pOFT->SetOctreeCell({});
			pOFT = next;
		}
		cell->SetLatestOFT({});
	}
}

bool MyLib::OctreeManager::Register(std::weak_ptr<MyLib::ObjectForTree> pOFT, const BoundingBox& worldAABB)
{
	uint32_t index = GetCellIndex(worldAABB);
	if (index >= m_cellCount) return false;

	// セルが未生成なら親ごと生成する
	if (!m_pCells[index])
	{
		CreateCell(index);
	}

	return m_pCells[index]->Push(pOFT);
}

void MyLib::OctreeManager::GetCollisionPairs(std::vector<std::pair<std::shared_ptr<MyLib::Collidable>, std::shared_ptr<MyLib::Collidable>>>& pairs)
{
	pairs.clear();
	if (!m_pCells[0]) return;

	std::list<std::shared_ptr<MyLib::Collidable>> stack;
	ScanTree(0, pairs, stack);
}

uint32_t MyLib::OctreeManager::GetCellIndex(const BoundingBox& worldAABB) const
{
	// ワールド座標からグリッド座標に変換する
	auto toGrid = [&](const Vector3& pos) -> std::tuple<uint32_t, uint32_t, uint32_t>
		{
			uint32_t divNum = 1u << m_level;

			auto clampAxis = [&](float p, float minV, float cellSize) -> uint32_t
				{
					float diff = p - minV;
					if (diff <= 0.0f) return 0;
					return std::min(static_cast<uint32_t>(diff / cellSize), divNum - 1);
				};

			return { clampAxis(pos.x, m_worldMin.x, m_cellSize.x),clampAxis(pos.y, m_worldMin.y, m_cellSize.y),clampAxis(pos.z, m_worldMin.z, m_cellSize.z) };
		};

	// 最小グリッド座標を取得
	auto [minX, minY, minZ] = toGrid(worldAABB.min);
	// 最大グリッド座標を取得
	auto [maxX, maxY, maxZ] = toGrid(worldAABB.max);

	uint32_t mortonMin = GetMortonNumber3D(minX, minY, minZ);
	uint32_t mortonMax = GetMortonNumber3D(maxX, maxY, maxZ);

	// 2点のビットが食い違い始める場所を特定する
	uint32_t xorResult = mortonMin ^ mortonMax;

	uint32_t shift = 0;
	uint32_t temp = xorResult;
	while (temp > 0)
	{
		temp >>= kMortonBitsPerLevel;
		shift++;
	}

	uint32_t targetLevel = m_level - shift;
	uint32_t cellMorton = mortonMax >> (shift * kMortonBitsPerLevel);

	// 1つ上の階層までのセルの総数(等比数列の和)
	uint32_t offset = (Pow8(targetLevel) - 1) / (kChildCellNum - 1);

	return offset + cellMorton;
}

uint32_t MyLib::OctreeManager::GetTotalCellCount(uint32_t level) const
{
	// レベルごとのセルの総数(等比数列の和)
	return (Pow8(level + 1) - 1) / (kChildCellNum - 1);
}

void MyLib::OctreeManager::CreateCell(uint32_t index)
{
	// 指定インデックスから親に向かって未生成のセルを作る
	while (index < m_cellCount && !m_pCells[index])
	{
		m_pCells[index] = std::make_shared<MyLib::OctreeCell>();
		if (index == 0) break;
		index = (index - 1) / kChildCellNum; // 親セルのインデックス
	}
}

void MyLib::OctreeManager::ScanTree(uint32_t cellIdx, std::vector<std::pair<std::shared_ptr<MyLib::Collidable>, std::shared_ptr<MyLib::Collidable>>>& pairs, std::list<std::shared_ptr<MyLib::Collidable>>& stack)
{
	auto& cell = m_pCells[cellIdx];
	if (!cell) return;

	// このセルのオブジェクトをリストに収集
	std::vector<std::shared_ptr<MyLib::Collidable>> cellObjects;

	std::shared_ptr<MyLib::ObjectForTree> pOFT = cell->GetLatestOFT().lock();

	while (pOFT)
	{
		if (std::shared_ptr<MyLib::Collidable> owner = pOFT->GetOwner().lock())
		{
			cellObjects.push_back(owner);
		}
		pOFT = pOFT->GetNext().lock();
	}

	// セル内のオブジェクト同士のペアを追加
	for (size_t i = 0; i < cellObjects.size(); ++i)
	{
		for (size_t j = i + 1; j < cellObjects.size(); ++j)
		{
			pairs.emplace_back(cellObjects[i], cellObjects[j]);
		}
	}

	// スタック(祖先セル)のオブジェクトとのペアを追加
	for (auto& obj : cellObjects)
	{
		for (auto& stackObj : stack)
		{
			pairs.emplace_back(obj, stackObj);
		}
	}

	// このセルのオブジェクトをスタックに積む
	for (auto& obj : cellObjects)
	{
		stack.push_back(obj);
	}

	// 子セルへ再帰
	for (uint32_t i = 0; i < kChildCellNum; ++i)
	{
		uint32_t childIdx = cellIdx * kChildCellNum + 1 + i;
		if (childIdx < m_cellCount && m_pCells[childIdx])
		{
			ScanTree(childIdx, pairs, stack);
		}
	}
	
	// スタックから自分の分を取り出す
	for (size_t i = 0; i < cellObjects.size(); ++i)
	{
		stack.pop_back();
	}

}
