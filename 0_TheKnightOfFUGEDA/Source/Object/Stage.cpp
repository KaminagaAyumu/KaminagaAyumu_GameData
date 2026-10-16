#include "Stage.h"
#include "Ground.h"
#include "Wall.h"
#include "../MyLib/ObjectManager.h"
#include "../MyLib/Component/Rigidbody.h"
#include "../MyLib/Component/Collision/Collidable.h"
#include "../MyLib/Component/Draw/Drawable3D.h"
#include "../MyLib/Collider/SphereCollider.h"
#include "../Utility/File/FileManager.h"
#include "../Utility/File/File.h"
#include "../Utility/Binary/TerrainResource.h"
#include <memory>

namespace
{
	// ファイルを読み込む際のパスの最大サイズ(文字数)
	constexpr size_t kFilePathMax = 256;
}

Stage::Stage()
{
}

Stage::~Stage()
{
}

void Stage::Init(int stageNo)
{
	MyLib::ObjectManager& objManager = MyLib::ObjectManager::GetInstance();

	m_pGround = std::make_shared<Ground>();
	objManager.AddObject(m_pGround);
	m_pGround->Init(stageNo);

	m_pWall = std::make_shared<Wall>();
	objManager.AddObject(m_pWall);
	m_pWall->Init(stageNo);

	wchar_t terrainFilePath[kFilePathMax];
	std::swprintf(terrainFilePath, kFilePathMax, L"Data/File/Binary/terrain_mesh_%d.bin", stageNo);
	
	TerrainResource terrain;

	if (terrain.Load(terrainFilePath))
	{
		auto [pGroundCollider, pWallCollider] = terrain.ConvertTerrainColliders();

		auto groundCol = m_pGround->GetComponent<MyLib::Collidable>().lock();
		auto wallCol = m_pWall->GetComponent<MyLib::Collidable>().lock();

		pGroundCollider->SetEnable(false);
		//pWallCollider->SetEnable(false);

#ifdef _DEBUG
		BoundingBox wallBox = pWallCollider->GetBoundingBox({},{});
		BoundingBox groundBox = pGroundCollider->GetBoundingBox({},{});
		printfDx(L"wall min(%.1f,%.1f,%.1f) max(%.1f,%.1f,%.1f)\n",
			wallBox.min.x, wallBox.min.y, wallBox.min.z, wallBox.max.x, wallBox.max.y, wallBox.max.z);
		printfDx(L"ground min(%.1f,%.1f,%.1f) max(%.1f,%.1f,%.1f)\n",
			groundBox.min.x, groundBox.min.y, groundBox.min.z, groundBox.max.x, groundBox.max.y, groundBox.max.z);
#endif

		groundCol->AddCollider(pGroundCollider);
		wallCol->AddCollider(pWallCollider);
	}
}

void Stage::Update()
{
}

void Stage::End()
{
	m_pGround->End();
	m_pWall->End();
}