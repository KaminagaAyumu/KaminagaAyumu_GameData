#include "LifeUI.h"
#include "../../Utility/File/FileManager.h"
#include "../../MyLib/ObjectFactory.h"
#include "../../MyLib/Component/Transform.h"

namespace
{
	constexpr int kPulseFrame = 60;

	constexpr float kPulseRange = 0.1f;

	const Vector2 kImageScale = { 0.25f,0.25f };
}

LifeUI::LifeUI() : 
	m_currentLife(-1)
{
}

void LifeUI::Init(const Vector2Int& leftPos, int margin, int maxLife)
{
	m_pFullFile = FileManager::GetInstance().GetImage(L"Data/File/Image/life.png", false);
	m_pEmptyFile = FileManager::GetInstance().GetImage(L"Data/File/Image/life_empty.png", false);

	for (int i = 0; i < maxLife; ++i)
	{
		Vector2Int pos = { leftPos.x + margin * i, leftPos.y };

		auto pObject = MyLib::ObjectFactory::CreateUIImage(pos);

		auto pImage = pObject->GetComponent<MyLib::UIImage>();
		if (auto pSharedImage = pImage.lock())
		{
			pSharedImage->SetImageFile(m_pFullFile);
			pSharedImage->SetScale(kImageScale);

			const bool isLeadingHeart = (i == maxLife - 1);
			if (isLeadingHeart)
			{
				pSharedImage->StartPulse(kPulseFrame, kPulseRange);
			}
		}
		pObject->Init();

		m_pHeartObjects.push_back(pObject);
		m_pHeartImages.push_back(pImage);
	}
	m_currentLife = maxLife;
}

void LifeUI::SetLife(int life)
{
	if (life == m_currentLife) return;
	m_currentLife = life;

	const int heartCount = static_cast<int>(m_pHeartImages.size());

	for (int i = 0; i < heartCount; ++i)
	{
		auto pImage = m_pHeartImages[i].lock();

		if (!pImage) continue;

		const bool isFull = (i < life);
		pImage->SetImageFile(isFull ? m_pFullFile : m_pEmptyFile);

		const bool isLeadingHeart = (i == life - 1);
		if (isLeadingHeart)
		{
			pImage->StartPulse(kPulseFrame, kPulseRange);
		}
		else
		{
			pImage->StopPulse();
		}
	}

}
