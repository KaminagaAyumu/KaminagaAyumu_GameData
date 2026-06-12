#pragma once
#include "../../MyLib/Collidable.h"

class BulletBase : public MyLib::Collidable
{
public:

	BulletBase(ObjectTag tag, MyLib::ColliderBase::ColliderType type);
	virtual ~BulletBase();

	virtual void Init()abstract;
	virtual void End()abstract;
	virtual void Update()abstract;
	virtual void Draw()const abstract;

	/// <summary>
	/// 弾が死んだかを返す
	/// </summary>
	/// <returns></returns>
	bool IsDead()const { return m_isDead; }

protected:
	// 弾が死んだか
	bool m_isDead;

};

