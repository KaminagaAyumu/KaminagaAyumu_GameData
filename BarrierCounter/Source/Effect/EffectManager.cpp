#include "EffectManager.h"
#include "Effect.h"
#include "EffekseerForDXLib.h"
#include <cassert>

namespace
{
	// 共通で使用するパスの文字列
	const std::wstring kCommonPathName = L"Data/Effect/";

	// リソースが存在していることを示す
	constexpr int kResourceExist = 1;
}

void EffectManager::LoadEffect(const std::wstring& path)
{
	// エフェクトのパスがすでにロード済みの場合
	if (m_effectResources.contains(path))
	{
		// リソースの参照カウンタを増加
		m_effectResources[path].refCounter++;
	}
	else
	{
		// リソースの参照カウンタを1にする
		m_effectResources[path].refCounter = kResourceExist;
		// パスに対応したエフェクトハンドルを取得
		m_effectResources[path].handle = LoadEffekseerEffect((kCommonPathName + path).c_str());
		// パスが見つからなかったらアサート
		assert(m_effectResources[path].handle >= 0 && "エフェクトのパスが存在しません");
	}
}

int EffectManager::DeleteEffect(const std::wstring& path)
{
	// 指定されたパスの要素を返す
	auto it = m_effectResources.find(path);
	assert(it != m_effectResources.end() && "エフェクト削除時にパスからハンドルの取得に失敗しました");
	
	// 参照カウンタを減らして、最後の参照だったら
	if (--it->second.refCounter <= 0)
	{
		// リソースのハンドルを開放
		DeleteEffekseerEffect(it->second.handle);
		// コンテナからも削除
		m_effectResources.erase(it);
		return 0; // 削除成功
	}
	else
	{
		return kResourceExist; // 削除しないとする
	}
}

void EffectManager::StopAllEffects()
{
	// 残っているエフェクトがある場合はすべてのエフェクトの終了処理を行う
	for (auto& effect : m_pEffects)
	{
		effect->End();
	}
	// エフェクトリストをクリア
	m_pEffects.clear();
}

std::weak_ptr<Effect> EffectManager::CreateEffect(const std::wstring& path, const Vector3& pos)
{
	// エフェクトを生成
	auto effect = std::make_shared<Effect>();

	// 指定されたパスの要素を返す
	auto it = m_effectResources.find(path);
	assert(it != m_effectResources.end() && "エフェクト生成時にパスからハンドルの取得に失敗しました");

	// エフェクトにハンドルをセット
	effect->SetHandle(it->second.handle);
	// エフェクトの初期化
	effect->Init();
	// 座標をセット
	effect->SetPos(pos);
	// エフェクトリストに追加
	m_pEffects.push_back(effect);
	// 生成したものを返す
	return effect;
}

EffectManager::EffectManager()
{
	// 初期状態は更新を行う
	m_isStop = false;
}

EffectManager::~EffectManager()
{
}

EffectManager& EffectManager::GetInstance()
{
	static EffectManager instance;
	return instance;
}

void EffectManager::Init()
{
	// エフェクトリソースのコンテナをクリア
	m_effectResources.clear();

	// エフェクトのリストをクリア
	m_pEffects.clear();

	// 更新を行う状態に初期化
	m_isStop = false;
}

void EffectManager::End()
{
	// 残っているエフェクトがある場合はすべてのエフェクトの終了処理を行う
	for (auto& effect : m_pEffects)
	{
		effect->End();
	}
	// エフェクトリストをクリア
	m_pEffects.clear();

	// ロードされているすべてのエフェクトをメモリから解放する
	for (auto& effectHandle : m_effectResources)
	{
		// リソースのハンドルを開放
		DeleteEffekseerEffect(effectHandle.second.handle);
	}
	// エフェクトのコンテナをクリア
	m_effectResources.clear();
}

void EffectManager::Update()
{
	// 更新を止める状態ならば以下の処理は行わない
	if (m_isStop) { return; }

	// Effekseerのエフェクトを現在の3D描画情報に適応
	Effekseer_Sync3DSetting();

	// エフェクトの更新
	for (auto& effect : m_pEffects)
	{
		effect->Update();
		// エフェクトが存在しない時
		if (!effect->IsAlive())
		{
			// エフェクトの終了処理を行う
			effect->End();
		}
	}

	// エフェクトの中で存在しないものを探す
	m_pEffects.remove_if([](std::shared_ptr<Effect> effect)
		{
			// エフェクトが消えた判定の時に削除
			return !effect->IsAlive();
		});

	// すべてのエフェクトを更新
	UpdateEffekseer3D();
}

void EffectManager::Draw() const
{
	// エフェクトはこの関数を呼ぶとすべて描画される
	DrawEffekseer3D();
}