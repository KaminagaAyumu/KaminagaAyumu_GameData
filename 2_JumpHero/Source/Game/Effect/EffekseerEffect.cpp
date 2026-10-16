#include "../../Utility/Geometry.h"
#include "../../Utility/Camera.h"
#include <memory>
#include "EffekseerEffect.h"
#include "EffekseerForDxLib.h"
#include "../Actor.h"


EffekseerEffect::EffekseerEffect() :
	m_effect(-1),
	m_pos{},
	m_isUseCamera(false)
{
}

EffekseerEffect::~EffekseerEffect()
{
	StopEffekseer2DEffect(m_effect);
}

void EffekseerEffect::Init(int handle, const Position2& pos)
{
	m_effect = PlayEffekseer2DEffect(handle);

	m_pos = pos;

	InitPos();
}

void EffekseerEffect::Init(int handle, const Position2& pos, std::weak_ptr<Camera> camera, bool isUseCamera)
{
	m_effect = PlayEffekseer2DEffect(handle);

	m_pos = pos;

	m_pCamera = camera;

	m_isUseCamera = isUseCamera;

	InitPos();
}

//void EffekseerEffect::Init(int handle, const Position2& pos, std::weak_ptr<Camera> camera, std::function<Position2()> provider, bool isUseCamera)
//{
//	m_effect = PlayEffekseer2DEffect(handle);
//
//	m_pos = pos;
//
//	m_pCamera = camera;
//
//	m_isUseCamera = isUseCamera;
//
//	m_pPosProvider = std::move(provider);
//
//	InitPos();
//}

void EffekseerEffect::Init(int handle, const Position2& pos, std::weak_ptr<Camera> camera, std::weak_ptr<Actor> provider, bool isUseCamera)
{
	m_effect = PlayEffekseer2DEffect(handle);

	m_pos = pos;

	m_pCamera = camera;

	m_isUseCamera = isUseCamera;

	m_pPosProvider = provider;

	InitPos();
}

void EffekseerEffect::Update()
{
	// 座標を指定するプロバイダが存在する場合
	if (auto provider = m_pPosProvider.lock())
	{
		// 座標をプロバイダのものにする
		m_pos = provider->GetPos();
	}

	// カメラが存在する場合
	if (auto camera = m_pCamera.lock())
	{
		// カメラを使う時のみスクロールの座標を設定
		if (m_isUseCamera)
		{
			// スクロールを含めて座標を設定
			float screenX = m_pos.x - camera->scroll.x;
			float screenY = m_pos.y - camera->scroll.y;
			SetPosPlayingEffekseer2DEffect(m_effect, screenX, screenY, 0);
		}
	}
	// カメラが存在しない場合は指定された座標から動かない
}

bool EffekseerEffect::IsAlive()const
{
	return IsEffekseer2DEffectPlaying(m_effect) == 0;
}

void EffekseerEffect::StopEffect() const
{
	StopEffekseer2DEffect(m_effect);
}

void EffekseerEffect::InitPos()
{
	// カメラが存在する場合
	if (auto camera = m_pCamera.lock())
	{
		// 座標を指定するプロバイダが存在する場合
		if (auto provider = m_pPosProvider.lock())
		{
			// 座標をプロバイダのものにする
			m_pos = provider->GetPos();
		}

		if (m_isUseCamera)
		{
			// スクロールを含めて座標を設定
			float screenX = m_pos.x - camera->scroll.x;
			float screenY = m_pos.y - camera->scroll.y;
			SetPosPlayingEffekseer2DEffect(m_effect, screenX, screenY, 0);
		}
		else
		{
			SetPosPlayingEffekseer2DEffect(m_effect, m_pos.x, m_pos.y, 0);
		}
	}
	else // カメラがない場合
	{
		// 指定された座標をそのまま設定
		SetPosPlayingEffekseer2DEffect(m_effect, m_pos.x, m_pos.y, 0);
	}
}
