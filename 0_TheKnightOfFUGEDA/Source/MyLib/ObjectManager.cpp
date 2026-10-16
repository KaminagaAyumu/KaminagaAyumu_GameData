#include "ObjectManager.h"
#include "GameObject.h"
#include <cassert>

MyLib::ObjectManager::ObjectManager() : 
	m_isUpdate(true)
{
}

MyLib::ObjectManager::~ObjectManager()
{
}

MyLib::ObjectManager& MyLib::ObjectManager::GetInstance()
{
	static ObjectManager instance;
	return instance;
}

void MyLib::ObjectManager::AddObject(std::shared_ptr<GameObject> object)
{
	// 既に追加されているかを確認
	bool isFound = (std::find(m_pGameObjects.begin(), m_pGameObjects.end(), object) != m_pGameObjects.end());

	if (isFound)
	{
		// 既に追加されていた場合アサート
		assert(false && "既に登録されているゲームオブジェクトが登録されました");
	}
	else
	{
		// 追加する
		m_pGameObjects.emplace_back(object);
	}
}

void MyLib::ObjectManager::Init()
{
	// ゲームオブジェクトがない場合何もしない
	if (m_pGameObjects.empty()) { return; }

	// すべてのゲームオブジェクトの初期化処理を行う
	for (auto& object : m_pGameObjects)
	{
		object->Init();
	}
}

void MyLib::ObjectManager::Update()
{
	if (!m_isUpdate)
	{
		// すべてのゲームオブジェクトの更新を行う
		for (auto& object : m_pGameObjects)
		{
			if (!object->IsDestroyed() && object->GetType() == MyLib::GameObject::Type::UI)
			{
				object->Update();
			}
			//object->Update();

			// オブジェクトが削除されたとき
			if (object->IsDestroyed())
			{
				// オブジェクトの終了処理を行う
				object->End();
			}
		}
	}
	else
	{
		// すべてのゲームオブジェクトの更新を行う
		for (auto& object : m_pGameObjects)
		{
			if (!object->IsDestroyed())
			{
				object->Update();
			}
			//object->Update();

			// オブジェクトが削除されたとき
			if (object->IsDestroyed())
			{
				// オブジェクトの終了処理を行う
				object->End();
			}
		}
	}



	// ゲームオブジェクトの中で削除されているものを探す
	m_pGameObjects.remove_if([](std::shared_ptr<GameObject> object)
		{
			// 削除されている際にリストから消去する
			return object->IsDestroyed();
		});
}

void MyLib::ObjectManager::End()
{
	// ゲームオブジェクトがない場合何もしない
	if (m_pGameObjects.empty()) { return; }

	// すべてのゲームオブジェクトの終了処理を行う
	for (auto& object : m_pGameObjects)
	{
		object->End();
	}

	// ゲームオブジェクトリストを初期化
	m_pGameObjects.clear();
}