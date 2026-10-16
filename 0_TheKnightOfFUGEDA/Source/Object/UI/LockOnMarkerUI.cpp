#include "LockOnMarkerUI.h"
#include "../../MyLib/ObjectFactory.h"
#include "../../MyLib/MyMath.h"
#include "../../Utility/File/FileManager.h"

namespace
{
	const Vector2 kMarkerScale = { 0.5f,0.5f };

	constexpr int kAppearFrame = 10;

	constexpr float kRotSpeed = 0.05f;
}

LockOnMarkerUI::LockOnMarkerUI() : 
	m_isClosing(false)
{
}

void LockOnMarkerUI::Show(std::weak_ptr<MyLib::Transform> worldTarget)
{
	m_pWorldTarget = worldTarget;

	if (!m_pGameObj)
	{
		m_pGameObj = MyLib::ObjectFactory::CreateUIImage(Vector2Int::Zero());
		m_pUIImage = m_pGameObj->GetComponent<MyLib::UIImage>();

		if (auto pUIImage = m_pUIImage.lock())
		{
			pUIImage->SetImageFile(FileManager::GetInstance().GetImage(L"Data/File/Image/lockon.png", false));
			pUIImage->StartAppearCenter(kAppearFrame);
			pUIImage->SetScale(kMarkerScale);
		}
		m_pGameObj->Init();

	}
	else
	{
		if (auto pUIImage = m_pUIImage.lock())
		{
			pUIImage->SetActive(true);
			pUIImage->StartFadeIn(kAppearFrame);
			pUIImage->StartAppearCenter(kAppearFrame);
			pUIImage->Start();
		}
	}

	m_isClosing = false;
}

void LockOnMarkerUI::Hide()
{
	if (!m_pGameObj || m_isClosing) return;

	if (auto pUIImage = m_pUIImage.lock())
	{
		pUIImage->StartFadeOut(kAppearFrame, true);
		pUIImage->StartCloseCenter(kAppearFrame, true);
	}
	m_isClosing = true;
}

void LockOnMarkerUI::Update()
{
	if (!m_pGameObj) return;

	auto pImage = m_pUIImage.lock();
	if (!pImage) return;

	if (m_isClosing && !pImage->IsAlive())
	{
		m_pGameObj->Destroy();
		m_pGameObj.reset();
		m_pUIImage.reset();
		return;
	}

	if (auto pTarget = m_pWorldTarget.lock())
	{
		Vector2Int screenPos = MyLib::WorldPosToScreenPos(pTarget->GetPos());

		if (auto pTransform = m_pGameObj->GetComponent<MyLib::Transform>().lock())
		{
			pTransform->SetScreenPos(screenPos);
		}

		pImage->SetRotation(pImage->GetRotation() + kRotSpeed);
		//printfDx(L"screen : %d,%d\n", screen.x, screen.y);
	}
}
