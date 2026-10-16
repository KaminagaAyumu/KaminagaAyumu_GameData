#pragma once
#include "../../MyLib/GameObject.h"
#include "../../MyLib/Component/Draw/UI/UIImage.h"
#include <memory>
#include <vector>

class File;

class LifeUI
{
public:

	LifeUI();
	virtual ~LifeUI() = default;

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="leftPos">左端のハートの座標</param>
	/// <param name="margin">ハートの間隔</param>
	/// <param name="maxLife">ハートの最大数</param>
	void Init(const Vector2Int& leftPos, int margin, int maxLife);

	void SetLife(int life);

private:

	std::vector<std::shared_ptr<MyLib::GameObject>> m_pHeartObjects;
	std::vector<std::weak_ptr<MyLib::UIImage>> m_pHeartImages;

	std::shared_ptr<File> m_pFullFile;
	std::shared_ptr<File> m_pEmptyFile;

	int m_currentLife;
};

