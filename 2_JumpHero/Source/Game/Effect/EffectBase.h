#pragma once
#include "../../Utility/Animation.h"
#include "../../Utility/Geometry.h"

class Camera;

/// <summary>
/// エフェクト管理クラスの基底クラス
/// </summary>
class EffectBase
{
public:
	EffectBase();
	virtual ~EffectBase() = default;

	virtual void Update()abstract;
	virtual void Draw(const std::weak_ptr<Camera>)abstract;

	bool IsAlive()const { return m_isAlive; }

protected:
	int m_handle; // 画像ハンドル

	Position2 m_pos; // 生成座標

	bool m_isAlive; // 存在フラグ

	Animation m_animation; // エフェクトを動かすためのアニメーションクラス

};

