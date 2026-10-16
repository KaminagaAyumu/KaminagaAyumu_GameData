#include "CameraManager.h"
#include "../MyLib/MyMath.h"
#include <cassert>

CameraManager::CameraManager()
{
}

CameraManager::~CameraManager()
{
}

CameraManager& CameraManager::GetInstance()
{
	static CameraManager instance;
	return instance;
}

void CameraManager::Entry(std::shared_ptr<MyLib::CameraComponent> camera)
{
	// 既に登録されているかを確認
	bool isFound = (std::find(m_pCameras.begin(), m_pCameras.end(), camera) != m_pCameras.end());

	if (isFound)
	{
		// 既に登録されていた場合アサート
		assert(false && "既に登録されているカメラが登録されました");
	}
	else
	{
		// 登録する
		m_pCameras.emplace_back(camera);
	}
}

void CameraManager::Exit(std::shared_ptr<MyLib::CameraComponent> camera)
{
	// 登録されているカメラコンポーネントを確認
	auto count = std::erase_if(m_pCameras, [camera](std::shared_ptr<MyLib::CameraComponent> target)
		{
			// 対象のカメラコンポーネントと一致したものを消去
			return target == camera;
		});
}

void CameraManager::Init()
{

}

void CameraManager::Update()
{
	// カメラが登録されていない場合処理をしない
	if (m_pCameras.empty())
	{
		return;
	}
	std::shared_ptr<MyLib::CameraComponent> mainCamera;
	mainCamera = m_pCameras.back();
	for (auto& camera : m_pCameras)
	{
		if (mainCamera->GetPriority() < camera->GetPriority())
		{
			mainCamera = camera;
		}
	}

	SetupCamera_Perspective(MyLib::ToRadian(mainCamera->GetFov()));
	SetCameraNearFar(mainCamera->GetNear(), mainCamera->GetFar());
	SetCameraPositionAndTarget_UpVecY(mainCamera->GetPosition(), mainCamera->GetTarget());
}

void CameraManager::End()
{

}
