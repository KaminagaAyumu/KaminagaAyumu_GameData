#pragma once
#include <string>

/// <summary>
/// アニメーションのデータ
/// </summary>
class AnimData
{
public:

	AnimData();
	virtual ~AnimData();

	void SetData(const std::wstring& animName, float animSpeed, int blendFrame, bool isLoop);

	
private:
	std::wstring m_animName;	// アニメーション名
	float m_animSpeed;			// アニメーションの速度
	int m_blendFrame;			// ブレンドするフレーム数
	bool m_isLoop;				// ループするかどうか
};

