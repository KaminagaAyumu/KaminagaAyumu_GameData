#pragma once
#include <string>
#include <map>

/// <summary>
/// Effekseerのデータを管理するクラス
/// </summary>
class EffekseerResourceManager
{
public:
	/// <summary>
	/// エフェクトをロードする
	/// </summary>
	/// <param name="path">エフェクトのパス</param>
	/// <returns>エフェクトのハンドル</returns>
	int LoadEffect(const std::wstring& path);

	/// <summary>
	/// エフェクトを消去する
	/// </summary>
	/// <param name="handle">エフェクトのハンドル</param>
	/// <returns>0:成功 1:まだ参照先があるのでリソースの削除はしない -1:削除に失敗</returns>
	int DeleteEffect(int handle);

private:
	struct EffekseerResourceInfo
	{
		int refCounter; // 参照カウンタ
		int handle; // エフェクトのハンドル
	};

	// エフェクトのリソースを管理するマップ(文字列はエフェクトのパス)
	std::map<std::wstring, EffekseerResourceInfo> m_effekseerResources;

};

