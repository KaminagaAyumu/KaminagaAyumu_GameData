#pragma once
#include "../../MyLib/GameObject.h"
#include "../../MyLib/Component/Transform.h"
#include "../../MyLib/Component/Draw/UI/UIImage.h"
#include <memory>


class LockOnMarkerUI
{
public:

	LockOnMarkerUI();
	virtual ~LockOnMarkerUI() = default;

	void Show(std::weak_ptr<MyLib::Transform> worldTarget);

	void Hide();

	void Update();

private:
	// マーカーUI用オブジェクト
	std::shared_ptr<MyLib::GameObject> m_pGameObj;
	// マーカーUIコンポーネント
	std::weak_ptr<MyLib::UIImage> m_pUIImage;
	// マーカーUIの座標コンポーネント
	std::weak_ptr<MyLib::Transform> m_pWorldTarget;
	// UIを閉じているかどうか
	bool m_isClosing;
};

