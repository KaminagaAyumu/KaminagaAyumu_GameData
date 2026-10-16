#pragma once
#include "BinaryResource.h"
#include "../../MyLib/Collider/GroundMeshCollider.h"
#include "../../MyLib/Collider/WallMeshCollider.h"
#include <memory>

class TerrainResource : public BinaryResource
{
public:
	TerrainResource();
	virtual ~TerrainResource();

	std::pair<std::shared_ptr<MyLib::GroundMeshCollider>, std::shared_ptr<MyLib::WallMeshCollider>> ConvertTerrainColliders();


};

