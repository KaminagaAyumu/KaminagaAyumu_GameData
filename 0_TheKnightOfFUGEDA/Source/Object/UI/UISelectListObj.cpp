#include "UISelectListObj.h"
#include "../../MyLib/Component/Draw/UI/UISelectList.h"

UISelectListObj::UISelectListObj()
{
	AddComponent<MyLib::UISelectList>();	// 2D描画コンポーネントを追加
}

UISelectListObj::~UISelectListObj()
{
}

void UISelectListObj::Init()
{
	GameObject::Init();
}
