#include "UITelopObj.h"
#include "../../MyLib/Component/Draw/UI/UITelop.h"

UITelopObj::UITelopObj()
{
	AddComponent<MyLib::UITelop>();	// 2D描画コンポーネントを追加
}

UITelopObj::~UITelopObj()
{
}

void UITelopObj::Init()
{
	GameObject::Init();
}
