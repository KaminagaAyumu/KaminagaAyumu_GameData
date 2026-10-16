#pragma once
#include "../MyLib/GameObject.h"

/// <summary>
/// スカイボックスクラス
/// </summary>
class Skybox : public MyLib::GameObject
{
public:

	/// <summary>
	/// スカイボックスの種類
	/// </summary>
	enum class Type
	{
		Morning,
		Noon,
		Evening,
		Night,
	};


public:

	Skybox();
	virtual ~Skybox();

	void Init(Type type);
	void Update();
	void End();

private:

	// 回転の角度
	float m_angle;

};

