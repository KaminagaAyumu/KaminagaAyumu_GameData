#pragma once
#include "../Geometry/Vector3.h"
#include "../Geometry/BoundingBox.h"
#include <vector>
#include <list>
#include <memory>

namespace MyLib
{
	class OctreeCell;
	class ObjectForTree;
	class Collidable;
	/// <summary>
	/// 八分木空間を管理するクラス
	/// </summary>
	class OctreeManager
	{
	public:
		OctreeManager();
		virtual ~OctreeManager() = default;

		bool Init(uint32_t level, const BoundingBox& worldAABB);

		void RemoveAll();

		bool Register(std::weak_ptr<ObjectForTree> pOFT, const BoundingBox& worldAABB);

		void GetCollisionPairs(std::vector<std::pair<std::shared_ptr<Collidable>, std::shared_ptr<Collidable>>>& pairs);

	private:

		uint32_t GetCellIndex(const BoundingBox& worldAABB) const;

		uint32_t GetTotalCellCount(uint32_t level) const;

		/// <summary>
		/// 指定インデックスのセルを作成する
		/// </summary>
		/// <param name="index"></param>
		void CreateCell(uint32_t index);

		void ScanTree(uint32_t cellIdx, std::vector<std::pair<std::shared_ptr<Collidable>, std::shared_ptr<Collidable>>>& pairs, std::list<std::shared_ptr<Collidable>>& stack);

	private:

		uint32_t m_level;		// 空間分割数
		uint32_t m_cellCount;	// 分割するセルの総数
		Vector3 m_worldMin;		// ワールド座標の最少値
		Vector3 m_cellSize;		// セルあたりのサイズ

		// セル空間のコンテナ
		std::vector<std::shared_ptr<OctreeCell>> m_pCells;
	};
}

