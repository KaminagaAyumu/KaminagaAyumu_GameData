#include "GameObject.h"
#include "Component/Transform.h"
#include "ObjectManager.h"
#include <cassert>

MyLib::GameObject::GameObject(Type type)
{
	m_type = type;
	AddComponent<Transform>();
	m_isDestroyed = false;
}

MyLib::GameObject::~GameObject()
{
}

void MyLib::GameObject::Init()
{
	// コンポーネントの初期化処理を行う
	for (auto& [key, component] : m_components)
	{
		component->Init(weak_from_this());
	}
	
	// コンポーネントに干渉する初期化処理を行う
	for (auto& [key, component] : m_components)
	{
		component->Start();
	}
}

void MyLib::GameObject::Update()
{
	// コンポーネントの更新処理を行う
	for (auto& [key, component] : m_components)
	{
		component->Update();
	}
}

void MyLib::GameObject::End()
{
	// コンポーネントの終了処理を行う
	for (auto& [key, component] : m_components)
	{
		component->End();
	}
	// コンポーネントの内容を初期化する
	m_components.clear();
}
