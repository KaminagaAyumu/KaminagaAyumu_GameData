#pragma once
#include <memory>
#include "../Geometry/Vector3.h"
#include "../Geometry/Quaternion.h"

namespace MyLib
{
	class Rigidbody;

	/// <summary>
	/// 押し戻し情報
	/// </summary>
	struct PushBackInfo
	{
		std::shared_ptr<Rigidbody> rigidbodyA;
		std::shared_ptr<Rigidbody> rigidbodyB;
		Vector3 normal;
		float depth = 0.0f;
	};

	/// <summary>
	/// ワールド情報
	/// </summary>
	struct WorldInfo
	{
		Position3 pos;
		Quaternion rotation;
	};
}
