#include "UITextObj.h"
#include "../../MyLib/Component/Draw/UI/UIText.h"

UITextObj::UITextObj()
{
	AddComponent<MyLib::UIText>();	// 2D描画コンポーネントを追加
}

UITextObj::~UITextObj()
{
}

void UITextObj::Init()
{
	GameObject::Init();
}
