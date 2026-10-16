#pragma once
#include "../../MyLib/GameObject.h"
#include "../../MyLib/Component/Transform.h"
#include "../../Utility/CSV/EnemyResource.h"
#include <list>
#include <vector>
#include <memory>

class EnemyBase;

/// <summary>
/// 敵を管理するクラス
/// </summary>
class EnemyManager
{
public:

	/// <summary>
	/// 敵のタイプ
	/// </summary>
	enum class EnemyType
	{
		BulletEnemy,
		SkullEnemy
	};

public:

	EnemyManager();
	virtual ~EnemyManager();

	void Init(int stageNo);
	void Update();
	void End();

	// 敵を生成する(今後引数にステージの情報を入れる予定)
	void CreateEnemy(std::shared_ptr<MyLib::GameObject> player);

	bool IsEnemyDeadAll()const;

	void SetCanAct(bool canAct);
	
	/// <summary>
	/// 現在のプレイヤー座標から見た最も近い敵を探す
	/// </summary>
	/// <param name="playerPos">プレイヤーの座標</param>
	/// <param name="cameraPos">カメラの座標</param>
	/// <param name="fovDegree">カメラのFov</param>
	/// <returns>ターゲットとする敵のTransform</returns>
	std::weak_ptr<MyLib::Transform> GetNearEnemyTransform(const Vector3& playerPos, const Vector3& cameraPos, float fovDegree);

	/// <summary>
	/// ロックオン時、現在のロックオン対象から見た最も近い敵にロックオンの対象を変える
	/// </summary>
	/// <param name="playerPos">プレイヤーの座標</param>
	/// <param name="cameraPos">カメラの座標</param>
	/// <param name="fovDegree">カメラのFov</param>
	/// <param name="currentEnemyPos">現在のロックオン対象</param>
	/// <param name="isLeft">true : 右方向をチェックする false : 左方向をチェックする</param>
	/// <returns>新たなターゲット</returns>
	std::weak_ptr<MyLib::Transform> ReGetNearEnemyTransform(const Vector3& playerPos, const Vector3& cameraPos, float fovDegree, const Vector3& currentEnemyPos, bool isLeft);

	/// <summary>
	/// 現在のフレーム内で敵が死んだ数を返す
	/// </summary>
	/// <returns></returns>
	const int GetDeadCountThisFrame() const { return m_deadCountThisFrame; }

private:

	// 敵のコンテナ(生成管理などに使用)
	std::list<std::weak_ptr<EnemyBase>> m_pEnemies;

	// 敵の配置などのデータを管理するCSVデータクラス
	EnemyResource m_enemyResource;

	// 現在のフレーム内で敵が死んだ数
	int m_deadCountThisFrame;
};

