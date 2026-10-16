#pragma once
#include <functional>

class Camera;
class Actor;

/// <summary>
/// Effekseerのエフェクトを生成するクラス
/// </summary>
class EffekseerEffect
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	EffekseerEffect();
	~EffekseerEffect();

	/// <summary>
	/// カメラが存在しない場合の初期化処理
	/// </summary>
	/// <param name="handle">エフェクトのリソースハンドル</param>
	/// <param name="pos">エフェクトを表示する座標</param>
	void Init(int handle, const Position2& pos);

	/// <summary>
	/// カメラが存在する場合の初期化処理
	/// </summary>
	/// <param name="handle">エフェクトのリソースハンドル</param>
	/// <param name="pos">エフェクトを表示する座標</param>
	/// <param name="camera">カメラのポインタ</param>
	/// <param name="isUseCamera">カメラを使うかどうか</param>
	void Init(int handle, const Position2& pos, std::weak_ptr<Camera> camera, bool isUseCamera);

	/// <summary>
	/// 座標を指定して表示する際の初期化処理
	/// </summary>
	/// <param name="handle">エフェクトのリソースハンドル</param>
	/// <param name="pos">エフェクトを表示する座標</param>
	/// <param name="camera">カメラのポインタ</param>
	/// <param name="provider">座標を示すオブジェクト</param>
	/// <param name="isUseCamera">カメラを使うかどうか</param>
//	void Init(int handle, const Position2& pos, std::weak_ptr<Camera> camera, std::function<Position2()> provider, bool isUseCamera);
	void Init(int handle, const Position2& pos, std::weak_ptr<Camera> camera, std::weak_ptr<Actor> provider, bool isUseCamera);

	void Update();

	bool IsAlive()const;

	/// <summary>
	/// エフェクトを外部から止める処理
	/// </summary>
	void StopEffect()const;

private:
	int m_effect; // 実際に表示するエフェクト

	Position2 m_pos; // 表示する座標

	std::weak_ptr<Camera> m_pCamera; // カメラがある際に使う

	std::weak_ptr<Actor> m_pPosProvider; // 座標を指定する際に使う

	bool m_isUseCamera; // カメラを使うかどうか

	/// <summary>
	/// 座標を初期化する
	/// </summary>
	void InitPos();
};

