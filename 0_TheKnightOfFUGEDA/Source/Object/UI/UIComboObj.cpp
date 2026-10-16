#include "UIComboObj.h"
#include "../../MyLib/Component/Draw/UI/UICombo.h"

UIComboObj::UIComboObj()
{
	AddComponent<MyLib::UICombo>();	// 2D描画コンポーネントを追加
}

UIComboObj::~UIComboObj()
{
}

void UIComboObj::Init()
{
	GameObject::Init();
}
