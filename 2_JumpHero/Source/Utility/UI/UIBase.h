#pragma once

/// <summary>
/// UI関連の基底クラス
/// </summary>
class UIBase
{
public:
	virtual ~UIBase() = default;
	virtual void Update()abstract;
	virtual void Draw()const abstract;
	/// <summary>
	/// UIが存在しているかの判定を行う
	/// </summary>
	/// <returns>true : 存在している false : 存在しない</returns>
	virtual bool IsAlive()const { return true; }
};

