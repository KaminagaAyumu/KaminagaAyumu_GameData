#include "FadeManager.h"
#include "Game.h"
#include "DxLib.h"

namespace
{
	int kFadeTime = 60; // 1秒後にフェードが終わる
	int kMaxFadeAlpha = 255; // 画像の透明度の最大値(不透明)
}

FadeManager::FadeManager() :
	m_fadeState(FadeState::FadeOut), // フェードの状態を初期化
	m_fadeAlpha(kMaxFadeAlpha), // フェードの透明度を初期化（完全に黒い状態）
	m_fadeCount(0) // フェードのカウントを初期化
{
}

FadeManager::~FadeManager()
{
}

void FadeManager::Init()
{
	m_fadeState = FadeState::FadeOut; // フェードの状態を初期化
	m_fadeAlpha = kMaxFadeAlpha; // フェードの透明度を初期化（完全に黒い状態）
	m_fadeCount = 0; // フェードのカウントを初期化
}

void FadeManager::Update()
{
	switch (m_fadeState)
	{
	case FadeState::None:
		m_fadeCount = 0; // フェードカウントをリセット
		m_fadeAlpha = 0; // 透明度をリセット
		break;
	case FadeState::FadeIn:
		m_fadeCount++; // フェードカウントを増加
		if (m_fadeCount >= kFadeTime) // フェード時間に達したら
		{
			m_fadeState = FadeState::None; // フェード状態を終了
		}
		else
		{
			m_fadeAlpha = kMaxFadeAlpha * m_fadeCount / kFadeTime; // フェードの透明度を計算
		}
		break;
	case FadeState::FadeOut:
		m_fadeCount++; // フェードカウントを増加
		if (m_fadeCount >= kFadeTime) // フェード時間に達したら
		{
			m_fadeState = FadeState::None; // フェード状態を終了
		}
		else
		{
			m_fadeAlpha = kMaxFadeAlpha - (kMaxFadeAlpha * m_fadeCount / kFadeTime); // フェードの透明度を計算
		}
		break;
	default:
		break;
	}
}

void FadeManager::Draw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeAlpha);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}	

void FadeManager::StartFadeIn()
{
	m_fadeState = FadeState::FadeIn; // フェードの状態をフェードインに設定
}

void FadeManager::StartFadeOut()
{
	m_fadeState = FadeState::FadeOut; // フェードの状態をフェードアウトに設定
}
