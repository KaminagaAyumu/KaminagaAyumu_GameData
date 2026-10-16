#pragma once
#include "CameraBase.h"
#include "../../MyLib/Component/Transform.h"

/// <summary>
/// プレイヤーのカメラ
/// </summary>
class PlayerCamera : public CameraBase
{
public:
	PlayerCamera();
	virtual ~PlayerCamera() = default;

	void Init()override;
	void Update()override;
	void End()override;

	void SetPos(const Vector3& pos);

	/// <summary>
	/// ターゲットを指定する
	/// </summary>
	/// <param name="target"></param>
	void SetTarget(std::weak_ptr<MyLib::Transform> target);

	/// <summary>
	/// プレイヤーの正面を向く
	/// </summary>
	void LookForward();

	void SetLookAtOffset(const Vector3& offset) { m_lookAtOffset = offset; }

	void SetLockOnTarget(std::weak_ptr<MyLib::Transform> target);
	void ClearLockOn();

	bool IsLockOn() { return m_mode == CameraMode::LockOn; }

	Vector3 GetEyePos() const { return m_eye; }
	float GetFovDegree()const;

private:

	/// <summary>
	/// カメラの状態
	/// </summary>
	enum class CameraMode
	{
		Free,
		TurningToTarget,
		LockOn
	};

private:
	std::weak_ptr<MyLib::Transform> m_pTarget;
	std::weak_ptr<MyLib::Transform> m_pLockOnTarget;

	// 回転角度
	float m_yaw;
	float m_pitch;
	float m_distance;
	float m_fov;

	Vector3 m_lookAtOffset;

	Vector3 m_eye;
	Vector3 m_targetEye;
	Vector3 m_lookAt;
	Vector3 m_targetLookAt;

	// 補完してカメラ更新を行う際の変数
	float m_targetYaw;
	float m_targetPitch;
	float m_targetDistance;
	float m_targetFov;

	// ロックオン時の移動オフセット
	float m_lockOnYawOffset;
	float m_lockOnPitchOffset;
	
	CameraMode m_mode;

private:

	static Vector3 CalcEyePosition(const Vector3& lookAt, float yaw, float pitch, float distance);

	void UpdateCameraPosition();

	void UpdateLockOn();

};

