#include "UIObjBase.h"

UIObjBase::UIObjBase() : 
	GameObject(MyLib::GameObject::Type::UI)
{
}

UIObjBase::~UIObjBase()
{
}

void UIObjBase::Init()
{
	GameObject::Init();
}

void UIObjBase::Update()
{
	GameObject::Update();
}

void UIObjBase::End()
{
	GameObject::End();
}