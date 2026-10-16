#pragma once
#include <memory>
#include <list>
#include "../../MyLib/MyLib.h"

class BulletBase;
//class Collidable;

/// <summary>
/// 弾を管理するクラス
/// </summary>
class BulletManager
{
public:
	BulletManager();
	virtual ~BulletManager();

	void Init();
	void End();
	void Update();
	void Draw()const;

	/// <summary>
	/// 弾を生成する
	/// </summary>
	/// <param name="pos">生成座標</param>
	/// <param name="dir">進む向き</param>
	/// <param name="owner">生成するオブジェクト</param>
	void CreateBullet(const Vector3& pos, const Vector3& dir, std::shared_ptr<MyLib::Collidable> owner);

private:
	// 弾のモデルハンドル
	int m_modelHandle;

	// ゲーム中の弾リスト
	std::list<std::shared_ptr<BulletBase>> m_pBullets;

};

