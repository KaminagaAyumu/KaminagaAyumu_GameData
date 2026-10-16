#include <vector>
#include <list>
#include <memory>
#include "EffectManager.h"
#include "EffekseerEffect.h"
#include "EffekseerResourceManager.h"
#include <cassert>
#include <string>
#include "EffekseerForDxLib.h"


namespace
{
	// 画像ファイル名の配列
	const std::wstring kGraphFileName[] =
	{
		L"data/effect/coin_get.efk", // コイン取得
		L"data/effect/impact.efk", // 強化メダル
		L"data/effect/star.efk", // 強化メダル
		L"data/effect/powerup_1.efk", // パワーアップ
		L"data/effect/powerup_2.efk", // パワーアップ
		L"data/effect/jump.efk", // ジャンプ
		L"data/effect/change_coin.efk", // 敵をアイテム化するゲージがたまった時
		L"data/effect/change_coin_start.efk", // 敵をアイテム化したとき
		L"data/effect/balloon_get.efk",
		L"data/effect/blur.efk",
	};

	// エフェクトデータの数が違うときにエラーを出せるようにする
	static_assert(static_cast<int>(Types::EffectType::EffectNum) == _countof(kGraphFileName), "エフェクト数の定義が間違っています");
}

EffectManager::EffectManager(std::shared_ptr<EffekseerResourceManager> effectResourceManager) : 
	m_pEffekseerResourceManager(effectResourceManager)
{
	m_effekseerEffects.clear();

	for (int i = 0; i < static_cast<int>(Types::EffectType::EffectNum); i++)
	{
		int handle = m_pEffekseerResourceManager->LoadEffect(kGraphFileName[i]);
		m_effekseerResourceHandles.push_back(handle);
	}
}

EffectManager::~EffectManager()
{
	m_effekseerEffects.clear();

	for (auto& effectHandle : m_effekseerResourceHandles)
	{
		m_pEffekseerResourceManager->DeleteEffect(effectHandle);
	}
	m_effekseerResourceHandles.clear();
}

void EffectManager::Update()
{
	if (m_effekseerEffects.empty())
	{
		return;
	}

	for (auto& effect : m_effekseerEffects)
	{
		effect->Update();
	}

	// Effekseerにより再生中のエフェクトを更新する。
	//UpdateEffekseer2D();

	// Effekseerのエフェクトの削除処理
	m_effekseerEffects.remove_if(
		[](std::shared_ptr<EffekseerEffect> effect)
		{
			return !effect->IsAlive();
		}
	);
}

void EffectManager::Draw()
{
	
}

void EffectManager::CreateEffekseerEffect(Types::EffectType type, const Position2& pos, bool isUseCamera)
{
	auto effect = std::make_shared<EffekseerEffect>();
	// カメラがセットされている場合
	if (auto camera = m_pCamera.lock())
	{
		effect->Init(m_effekseerResourceHandles[(int)type], pos,camera,isUseCamera);
	}
	else // カメラがセットされていない場合
	{
		effect->Init(m_effekseerResourceHandles[(int)type], pos);
	}
	m_effekseerEffects.push_back(effect);
}

std::weak_ptr<EffekseerEffect> EffectManager::CreateEffekseerEffectWithProvider(Types::EffectType type, const Position2& pos, std::weak_ptr<Actor> provider, bool isUseCamera)
{
	auto effect = std::make_shared<EffekseerEffect>();
	// カメラがセットされている場合
	if (auto camera = m_pCamera.lock())
	{
		effect->Init(m_effekseerResourceHandles[(int)type], pos, camera, provider, isUseCamera);
	}
	else // カメラがセットされていない場合
	{
		effect->Init(m_effekseerResourceHandles[(int)type], pos);
	}
	m_effekseerEffects.push_back(effect);
	return effect;
}
