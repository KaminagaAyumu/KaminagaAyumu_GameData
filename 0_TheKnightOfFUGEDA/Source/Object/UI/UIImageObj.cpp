#include "UIImageObj.h"
#include "../../MyLib/Component/Draw/UI/UIImage.h"

UIImageObj::UIImageObj()
{
	AddComponent<MyLib::UIImage>();	// 2D描画コンポーネントを追加
}

UIImageObj::~UIImageObj()
{
}

void UIImageObj::Init()
{
	GameObject::Init();
}
