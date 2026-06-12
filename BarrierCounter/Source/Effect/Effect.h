#pragma once
#include "../Geometry/Vector3.h"

class Effect
{
public:
	Effect();
	virtual ~Effect();

	void Init();
	void End();
	void Update();

	/// <summary>
	/// エフェクトのハンドルをセットする
	/// </summary>
	/// <param name="handle">リソースハンドル</param>
	void SetHandle(int handle);

	/// <summary>
	/// エフェクトを停止する
	/// </summary>
	void StopEffect();

	/// <summary>
	/// エフェクトの表示座標をセットする
	/// </summary>
	/// <param name="pos">表示座標</param>
	void SetPos(const Vector3& pos);

	/// <summary>
	/// エフェクトが存在しているかどうかを返す
	/// </summary>
	/// <returns></returns>
	bool IsAlive()const;

private:

	// エフェクトのハンドル
	int m_effectHandle;

	// エフェクトの座標
	Vector3 m_pos;

};

