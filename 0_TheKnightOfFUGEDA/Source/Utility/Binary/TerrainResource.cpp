#include "TerrainResource.h"
#include "../../Geometry/Vector3.h"

TerrainResource::TerrainResource()
{
}

TerrainResource::~TerrainResource()
{
}

std::pair<std::shared_ptr<MyLib::GroundMeshCollider>, std::shared_ptr<MyLib::WallMeshCollider>> TerrainResource::ConvertTerrainColliders()
{
	// 床の頂点の数を読み込む
	int groundVerticesCount = Read<int>();
	// 床の頂点の数を保存する
	std::vector<Vector3> groundVertices = ReadVector<Vector3>(groundVerticesCount);

	// 床のインデックスを読み込む
	int groundIndicesCount = Read<int>();
	// 床のインデックスを保存する
	std::vector<int> groundIndices = ReadVector<int>(groundIndicesCount);

	// 壁の頂点の数を読み込む
	int wallVerticesCount = Read<int>();
	// 壁の頂点の数を保存する
	std::vector<Vector3> wallVertices = ReadVector<Vector3>(wallVerticesCount);

	// 壁のインデックスを読み込む
	int wallIndicesCount = Read<int>();
	// 壁のインデックスを保存する
	std::vector<int> wallIndices = ReadVector<int>(wallIndicesCount);

	// 行のデータを開放する
	ReleaseRawData();

	// メッシュコライダーを作成
	std::shared_ptr<MyLib::GroundMeshCollider> pGroundMeshCollider = std::make_shared<MyLib::GroundMeshCollider>(MyLib::ColliderBase::ObjectTag::Stage, std::move(groundVertices), std::move(groundIndices), false);
	std::shared_ptr<MyLib::WallMeshCollider> pWallMeshCollider = std::make_shared<MyLib::WallMeshCollider>(MyLib::ColliderBase::ObjectTag::Stage, std::move(wallVertices), std::move(wallIndices), false);

	return { pGroundMeshCollider, pWallMeshCollider };
}
