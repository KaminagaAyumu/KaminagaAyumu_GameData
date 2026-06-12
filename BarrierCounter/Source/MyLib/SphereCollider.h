#pragma once
#include "ColliderBase.h"

namespace MyLib
{

	class SphereCollider : public ColliderBase
	{
	public:
		SphereCollider();
		virtual ~SphereCollider();

		// 円の半径
		float m_radius;

	};
}

