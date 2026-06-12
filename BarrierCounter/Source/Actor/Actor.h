#pragma once
#include "../MyLib/Collidable.h"
#include "../Common/ObjectTag.h"

class Actor abstract : public MyLib::Collidable
{
public:

	Actor(ObjectTag tag, MyLib::ColliderBase::ColliderType type);
	virtual ~Actor();

	virtual void Init()abstract;
	virtual void End()abstract;
	virtual void Update()abstract;
	virtual void Draw()const abstract;

private:




};

