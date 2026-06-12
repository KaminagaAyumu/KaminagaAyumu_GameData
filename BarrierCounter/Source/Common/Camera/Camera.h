#pragma once

class Camera
{
public:

	Camera();
	virtual ~Camera();


	void Init();

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="target">カメラの注視点</param>
	/// <param name="fovDegree">カメラの視野角</param>
	/// <note>視野角、near、farをまとめた構造体を作ったほうがいいかも</note>
	void Update(Vector3 target, float fovDegree);

	/// <summary>
	/// 座標とターゲットの更新のみ行う処理
	/// </summary>
	/// <param name="target"></param>
	/// <param name="fovDegree"></param>
	void UpdateNoRotation(Vector3 target, float fovDegree);

	/// <summary>
	/// カメラを揺らす状態の時に行う更新処理
	/// </summary>
	void ShakeUpdate();

	void Draw();

	/// <summary>
	/// カメラを2点の間で補完する処理
	/// </summary>
	/// <param name="from">始点</param>
	/// <param name="to">終点</param>
	void LerpUpdate(Vector3 from, Vector3 to, float speed);

	/// <summary>
	/// カメラの角度を取得する
	/// </summary>
	/// <returns></returns>
	const float GetAngle()const { return m_angle; }

	/// <summary>
	/// カメラの正面方向ベクトルを取得する
	/// </summary>
	/// <returns></returns>
	const Vector3& GetCameraDir()const { return m_targetLocalDir; }

	/// <summary>
	/// 指定座標がカメラの範囲内かどうかを判定する
	/// </summary>
	/// <param name="target">指定座標</param>
	/// <param name="radius">半径(指定がない場合は0)</param>
	/// <returns>true : 範囲内 false : 範囲外</returns>
	bool IsInCamera(const Vector3& target, float radius = 0.0f) const;

	/// <summary>
	/// カメラを揺らす処理
	/// </summary>
	/// <param name="power">カメラの揺れる力</param>
	/// <param name="shakeFrame">カメラが揺れるフレーム数</param>
	void ShakeCamera(float power, int shakeFrame);

private:


	enum class CameraState
	{
		Default,
		Long,
	};

	// 基準の回転量
	float m_angle;

	// ターゲットからの距離の割合
	float m_targetDisRate;

	// カメラの座標
	Vector3 m_pos;

	// カメラの補正座標
	Vector3 m_lerpPos;

	// ターゲットのローカルの向き
	Vector3 m_targetLocalDir;

	// カメラとターゲットの距離
	float m_targetDistance;

	// カメラの状態
	CameraState m_state;
	
	// カメラが揺れているフレーム数
	int m_shakeFrameCount;

	// カメラの揺れる力
	float m_shakePower;
};