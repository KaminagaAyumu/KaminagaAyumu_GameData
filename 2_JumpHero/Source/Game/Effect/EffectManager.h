#pragma once
#include "../../Utility/Geometry.h"
#include "../../Utility/GameType.h"
#include <functional>

class EffekseerResourceManager;
class EffekseerEffect;
class Camera;

/// <summary>
/// ゲーム内のエフェクトを管理するクラス
/// </summary>
class EffectManager
{
public:
	EffectManager(std::shared_ptr<EffekseerResourceManager> effectResourceManager);
	virtual ~EffectManager();

	void Update();

	void Draw();

	/// <summary>
	/// エフェクシアのエフェクトを生成する(指定座標から動かない)
	/// </summary>
	/// <param name="type">エフェクトの種類</param>
	/// <param name="pos">エフェクトの座標</param>
	void CreateEffekseerEffect(Types::EffectType type, const Position2& pos, bool isUseCamera);

	/// <summary>
	/// エフェクシアのエフェクトを生成する(指定座標から動かせる)
	/// </summary>
	/// <param name="type">エフェクトの種類</param>
	/// <param name="pos">エフェクトの座標</param>
	/// <param name="provider">エフェクトの座標を示す関数プロバイダ</param>
	std::weak_ptr<EffekseerEffect> CreateEffekseerEffectWithProvider(Types::EffectType type, const Position2& pos, std::weak_ptr<Actor> provider, bool isUseCamera);

	/// <summary>
	/// カメラをセットする
	/// </summary>
	/// <param name="camera">カメラのポインタ</param>
	void SetCamera(std::weak_ptr<Camera> camera) { m_pCamera = camera; };

private:
	std::vector<int> m_effectHandles;
	std::vector<int> m_effekseerResourceHandles;
	std::list<std::shared_ptr<EffekseerEffect>> m_effekseerEffects;
	std::shared_ptr<EffekseerResourceManager> m_pEffekseerResourceManager;
	std::weak_ptr<Camera> m_pCamera; // 画面がスクロールする際に使う

};

