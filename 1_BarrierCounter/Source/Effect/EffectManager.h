#pragma once
#include <string>
#include <map>
#include <list>
#include <memory>
#include "../Geometry/Vector3.h"

class Effect;

class EffectManager
{
public:
	virtual ~EffectManager(); // デストラクタ

	/// <summary>
	/// インスタンスを取得する
	/// </summary>
	/// <returns></returns>
	static EffectManager& GetInstance();

	void Init();
	void End();
	void Update();
	void Draw() const;

	/// <summary>
	/// エフェクトをロードする
	/// </summary>
	/// <param name="path">エフェクトのパス</param>
	void LoadEffect(const std::wstring& path);

	/// <summary>
	/// エフェクトをアンロードする
	/// </summary>
	/// <param name="path">エフェクトのパス</param>
	/// <returns>0:成功 1:まだ参照先があるのでリソースの削除はしない -1:削除に失敗</returns>
	int DeleteEffect(const std::wstring& path);

	/// <summary>
	/// すべてのエフェクトの再生を止める
	/// </summary>
	void StopAllEffects();

	/// <summary>
	/// エフェクトを生成する
	/// </summary>
	/// <param name="path">エフェクトのパス</param>
	/// <param name="pos">座標</param>
	/// <returns>生成したエフェクトのポインタ</returns>
	std::weak_ptr<Effect> CreateEffect(const std::wstring& path, const Vector3& pos);

	/// <summary>
	/// エフェクトの更新を止める
	/// </summary>
	void StopUpdate() { m_isStop = true; }

	/// <summary>
	/// エフェクトの更新を行う
	/// </summary>
	void StartUpdate() { m_isStop = false; }

private:
	EffectManager(); // コンストラクタをprivateで宣言
	EffectManager(const EffectManager&) = delete; // コピーコンストラクタを作れないようにする
	void operator=(const EffectManager&) = delete; // 代入演算子も使えないようにする

	struct EffectResourceInfo
	{
		int refCounter; // 参照カウンタ
		int handle; // エフェクトのハンドル
	};

	// エフェクトのリソースを管理するmap(文字列はエフェクトのパス)
	std::map<std::wstring, EffectResourceInfo> m_effectResources;

	// エフェクトを管理するlist
	std::list<std::shared_ptr<Effect>> m_pEffects;

	// 更新を止めるかどうか
	bool m_isStop;
};

