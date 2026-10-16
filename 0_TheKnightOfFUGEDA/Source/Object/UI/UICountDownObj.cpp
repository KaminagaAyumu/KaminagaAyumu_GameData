#include "UICountDownObj.h"
#include "../../MyLib/Component/Draw/UI/UICountDown.h"

UICountDownObj::UICountDownObj()
{
	AddComponent<MyLib::UICountDown>();	// 2D描画コンポーネントを追加
}

UICountDownObj::~UICountDownObj()
{
}

void UICountDownObj::Init()
{
	GameObject::Init();
}
