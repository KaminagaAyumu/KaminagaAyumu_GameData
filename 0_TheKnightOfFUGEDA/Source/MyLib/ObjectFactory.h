#pragma once
#include "../MyLib/Renderer.h"
#include "../Object/Enemy/EnemyManager.h"
#include <memory>

class Vector3;
class Vector2Int;

namespace MyLib
{
	class GameObject;
	class Transform;

	/// <summary>
	/// オブジェクトを生成するための関数群
	/// </summary>
	namespace ObjectFactory
	{
		/// <summary>
		/// プレイヤーを生成する
		/// </summary>
		/// <returns></returns>
		std::shared_ptr<GameObject> CreatePlayer();

		std::shared_ptr<GameObject> CreateEnemy(EnemyManager::EnemyType type);

		std::shared_ptr<GameObject> CreateItem();

		std::shared_ptr<GameObject> CreateSkybox();

		std::shared_ptr<GameObject> CreateBullet(const Vector3& pos, const Vector3& target, std::weak_ptr<GameObject> shooter);

		std::shared_ptr<GameObject> CreateCamera();

		std::shared_ptr<GameObject> CreateUIImage(const Vector2Int& pos);

		std::shared_ptr<GameObject> CreateUISelectList(const Vector2Int& pos, MyLib::Renderer::FontType type);

		std::shared_ptr<GameObject> CretateUITelop(const Vector2Int& pos, MyLib::Renderer::FontType type);

		std::shared_ptr<GameObject> CreateUICountDown(const Vector2Int& pos, MyLib::Renderer::FontType type);
		
		std::shared_ptr<GameObject> CreateUIText(const Vector2Int& pos, MyLib::Renderer::FontType type);

		std::shared_ptr<GameObject> CreateUICombo(const Vector2Int& pos, MyLib::Renderer::FontType type);
	};
}



