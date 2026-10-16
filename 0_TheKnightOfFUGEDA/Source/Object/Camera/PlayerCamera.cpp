#include "PlayerCamera.h"
#include "../../MyLib/Component/Camera/CameraComponent.h"
#include "../../MyLib/MyMath.h"
#include "../../Utility/Input.h"
#include <memory>
#include <algorithm>

namespace
{
	constexpr float kFovDegree	= 60.0f;
	constexpr float kNear		= 0.3f;
	constexpr float kFar		= 1000.0f;
	constexpr int	kPriority	= 1;

	// カメラを回転させるスピード
	constexpr float kYawRotSpeed = 0.05f;
	constexpr float kPitchRotSpeed = 0.045f;

	constexpr float kLerpSpeed = 0.1f;

	// 注視点を見る際の補正スピード
	constexpr float kLookAtFollowSpeed = 0.085f;

	constexpr float kEyeFollowSpeed = 0.095f;

	// ターゲットに視点を合わせる際の閾値
	constexpr float kTurnToTargetThreshold = 0.1f;

	// 初期ヨー値
	constexpr float kDefaultYaw = 0.0f;

	// 初期ピッチ値
	constexpr float kDefaultPitch = MyLib::ToRadian(15.0f);

	// 初期カメラ距離
	constexpr float kDefaultDistance = 5.0f;

	// X軸角度の制限
	constexpr float kPitchMin = MyLib::ToRadian(-10.0f);
	constexpr float kPitchMax = MyLib::ToRadian(60.0f);

	// 基準点から敵にどれだけ寄せるか
	constexpr float kLockOnFocusWeight = 0.5f;

	constexpr float kLockOnRotSpeed = 0.08f;

	constexpr float kLockOnBasePitch = MyLib::ToRadian(15.0f);

	constexpr float kLockOnPitchFactor = 0.04f;

	constexpr float kLockOnYawOffset = MyLib::ToRadian(20.0f);

	const Vector3 kCameraFirstPos = { 0.0f,1.0f,-5.0f };

	// ロックオン時のカメラのオフセット
	const Vector3 kLockOnOffset = { -0.2f, 1.1f, 2.5f };

	// カメラのY方向の向きの制限
	constexpr float kCameraMaxDirY = 0.2f;
	constexpr float kCameraMinDirY = -0.8f;

	// ロックオン時の距離の調整用
	constexpr float kLockOnBaseDistance = 4.0f;
	constexpr float kLockOnDistanceFactor = 0.05f;
	constexpr float kLockOnMinDistance = 5.0f;
	constexpr float kLockOnMaxDistance = 15.0f;

	// ロックオンの際にrequiredDist(プレイヤーと敵との距離)に応じてEyeの距離を離すようにする座標の最大値
	constexpr float kLockOnDisRateMax = 7.0f;
	// ロックオンの際にプレイヤーと敵の距離が近い際に離す距離
	constexpr float kLockOnOffsetDist = 6.0f;

	constexpr float kLockOnOffsetSpeed = 0.02f;
	constexpr float kLockOnOffsetMaxYaw = MyLib::ToRadian(40.0f);
	constexpr float kLockOnOffsetMaxPitch = MyLib::ToRadian(10.0f);
	constexpr float kLockOnOffsetRecenterSpeed = 0.05f;

	constexpr float kLockOnFovWidenFactor = 1.5f;
	constexpr float kLockOnMaxFov = 80.0f;
	constexpr float kLockOnFovSmoothSpeed = 0.08f;

	// ロックオン時に画面内に表示する最低距離
	constexpr float kLockOnFovMargin = 4.0f;
}

PlayerCamera::PlayerCamera() : 
	m_yaw(kDefaultYaw),
	m_pitch(kDefaultPitch),
	m_distance(kDefaultDistance),
	m_fov(kFovDegree),
	m_lookAtOffset(Vector3::Zero()),
	m_targetYaw(kDefaultYaw),
	m_targetPitch(kDefaultPitch),
	m_targetDistance(kDefaultDistance),
	m_targetFov(kFovDegree),
	m_lockOnYawOffset(kDefaultYaw),
	m_lockOnPitchOffset(kDefaultPitch),
	m_mode(CameraMode::Free)
{
}

void PlayerCamera::Init()
{
	auto pTransform = GetComponent<MyLib::Transform>().lock();		// Transformコンポーネントを取得
	auto pCamera = AddComponent<MyLib::CameraComponent>().lock();	// カメラコンポーネント追加

	pTransform->SetPos(kCameraFirstPos);

	pCamera->SetFov(kFovDegree);
	pCamera->SetNearFar(kNear, kFar);
	pCamera->SetPriority(kPriority);

	// コンポーネントの初期化処理を行う
	GameObject::Init();
}

void PlayerCamera::Update()
{
	Input& input = Input::GetInstance();

	auto stick = input.GetXInputData();

	//bool isOnstickRightSide = stick.rightStick.x != 0.0f;

	switch (m_mode)
	{
	case CameraMode::Free:
		m_targetYaw += -stick.rightStick.x * kYawRotSpeed;

		m_targetPitch += stick.rightStick.y * kPitchRotSpeed;

		m_targetPitch = std::clamp(m_targetPitch, kPitchMin, kPitchMax);

		m_targetDistance = kDefaultDistance;
		break;
	case CameraMode::TurningToTarget:
	{
		m_targetPitch = kDefaultPitch;

		float diff = fabsf(MyLib::NormalizeAngle(m_targetYaw - m_yaw));
		float pitchDiff = fabsf(MyLib::NormalizeAngle(m_targetPitch - m_pitch));
		if (diff < kTurnToTargetThreshold && pitchDiff < kTurnToTargetThreshold)
		{
			m_mode = CameraMode::Free;
		}
	}
		break;
	case CameraMode::LockOn:
		// オフセットによってカメラの移動を行えるようにする
		m_lockOnYawOffset += stick.rightStick.x * kLockOnOffsetSpeed;
		m_lockOnPitchOffset += stick.rightStick.y * kLockOnOffsetSpeed;
		m_lockOnYawOffset = std::clamp(m_lockOnYawOffset, -kLockOnOffsetMaxYaw, kLockOnOffsetMaxYaw);
		m_lockOnPitchOffset = std::clamp(m_lockOnPitchOffset, -kLockOnOffsetMaxPitch, kLockOnOffsetMaxPitch);

		if (stick.rightStick.x == 0.0f)
		{
			m_lockOnYawOffset = MyLib::LerpAngle(m_lockOnYawOffset, 0.0f, kLockOnOffsetRecenterSpeed);
		}

		if (stick.rightStick.y == 0.0f)
		{
			m_lockOnPitchOffset = std::lerp(m_lockOnPitchOffset, 0.0f, kLockOnOffsetRecenterSpeed);
		}

		UpdateLockOn();
		break;
	default:
		break;
	}

	m_yaw = MyLib::LerpAngle(m_yaw, m_targetYaw, kLerpSpeed);
	m_pitch = std::lerp(m_pitch, m_targetPitch, kLerpSpeed);
	m_pitch = std::clamp(m_pitch, kPitchMin, kPitchMax);
	m_distance = std::lerp(m_distance, m_targetDistance, kLerpSpeed);
	m_fov = std::lerp(m_fov, m_targetFov, kLockOnFovSmoothSpeed);

	UpdateCameraPosition();

	// コンポーネントの更新処理を行う
	GameObject::Update();
}

void PlayerCamera::End()
{
	// コンポーネントの終了処理を行う
	GameObject::End();
}

void PlayerCamera::SetPos(const Vector3& pos)
{
	auto pTransform = GetComponent<MyLib::Transform>().lock();

	pTransform->SetPos(pos);
}

void PlayerCamera::SetTarget(std::weak_ptr<MyLib::Transform> target)
{
	m_pTarget = target;

	auto pTransform = GetComponent<MyLib::Transform>().lock();

	auto pTarget = target.lock();
	if (!pTarget) return;

	// 注視点の座標
	Vector3 lookAt = pTarget->GetPos() + m_lookAtOffset;
	// 注視点から視点へのベクトル
	Vector3 toEye = pTransform->GetPos() - lookAt;

	m_distance = toEye.Length();

	// 距離が正常に存在する場合回転を設定する
	if (m_distance > 0.0f)
	{
		m_yaw = atan2f(toEye.x, -toEye.z);
		m_pitch = asinf(std::clamp(toEye.y / m_distance, -1.0f, 1.0f));
	}

	// 補正用情報も初期化する
	m_targetYaw = m_yaw;
	m_targetPitch = m_pitch;
	m_targetDistance = m_distance;
	m_lookAt = lookAt;
	m_targetLookAt = lookAt;

	UpdateCameraPosition();
}

void PlayerCamera::LookForward()
{
	// 一旦対象をターゲットとする
	auto playerTransform = m_pTarget.lock();
	if (!playerTransform) return;

	// プレイヤーの正面方向を取得する
	Vector3 dir = playerTransform->GetDir();

	m_targetYaw = atan2f(-dir.x, dir.z);

	m_mode = CameraMode::TurningToTarget;
}

void PlayerCamera::SetLockOnTarget(std::weak_ptr<MyLib::Transform> target)
{
	m_pLockOnTarget = target;
	m_mode = CameraMode::LockOn;
}

void PlayerCamera::ClearLockOn()
{
	m_pLockOnTarget.reset();
	m_mode = CameraMode::Free;

	// 注視点から視点へのベクトル
	Vector3 toEye = m_eye - m_lookAt;

	m_distance = toEye.Length();

	// 距離が正常に存在する場合回転を設定する
	if (m_distance > 0.0f)
	{
		m_yaw = atan2f(toEye.x, -toEye.z);
		m_pitch = asinf(std::clamp(toEye.y / m_distance, -1.0f, 1.0f));
	}

	// 補正用情報も初期化する
	m_targetYaw = m_yaw;
	m_targetPitch = m_pitch;
	m_targetDistance = m_distance;

	m_lockOnYawOffset = 0.0f;
	m_lockOnPitchOffset = 0.0f;
}

float PlayerCamera::GetFovDegree() const
{
	return kFovDegree;
}

Vector3 PlayerCamera::CalcEyePosition(const Vector3& lookAt, float yaw, float pitch, float distance)
{
	Vector3 offset;
	offset.x = distance * cosf(pitch) * sinf(yaw);
	offset.y = distance * sinf(pitch);
	offset.z = -distance * cosf(pitch) * cosf(yaw);

	return lookAt + offset;
}

void PlayerCamera::UpdateCameraPosition()
{
	auto target = m_pTarget.lock();
	if (!target) return;

	Vector3 baseLookAt = target->GetPos() + m_lookAtOffset;

	// ロックオン状態の処理
	if (m_mode == CameraMode::LockOn)
	{
		// ロックオンの対象のTransformが見つかったら、敵とプレイヤーのLookAtを補完する
		if (auto lockOn = m_pLockOnTarget.lock())
		{
			Vector3 enemyLookAt = lockOn->GetPos() + m_lookAtOffset;
			m_targetLookAt = Vector3::LerpVec3(baseLookAt, enemyLookAt, kLockOnFocusWeight);
		}
		else // 見つからなかったらプレイヤーのLookAtをそのまま適用する
		{
			m_targetLookAt = baseLookAt;
		}
	}
	else
	{
		m_targetLookAt = baseLookAt;
	}

	m_lookAt = Vector3::LerpVec3(m_lookAt, m_targetLookAt, kLookAtFollowSpeed);

	//m_eye = CalcEyePosition(m_lookAt, m_yaw, m_pitch, m_distance);

	// ロックオン状態の処理
	if (m_mode == CameraMode::LockOn)
	{
		// ロックオンの対象のTransformが見つかったら、敵とプレイヤーのLookAtを補完する
		if (auto lockOn = m_pLockOnTarget.lock())
		{
			// 何もしない
		}
		else // 見つからなかったらプレイヤーのLookAtをそのまま適用する
		{
			m_targetEye = CalcEyePosition(m_targetLookAt, m_targetYaw, m_targetPitch, m_targetDistance);
		}
	}
	else
	{
		m_targetEye = CalcEyePosition(m_targetLookAt, m_targetYaw, m_targetPitch, m_targetDistance);

		//m_targetEye = CalcEyePosition(m_lookAt, m_yaw, m_pitch, m_distance);
	}

	m_eye = Vector3::LerpVec3(m_eye, m_targetEye, kEyeFollowSpeed);

	auto pTransform = GetComponent<MyLib::Transform>().lock();
	pTransform->SetPos(m_eye);

	auto pCamera = GetComponent<MyLib::CameraComponent>().lock();
	pCamera->SetTarget(m_lookAt);

	pCamera->SetFov(m_fov);
}

void PlayerCamera::UpdateLockOn()
{
	auto player = m_pTarget.lock();
	auto lockOn = m_pLockOnTarget.lock();

	if (!player || !lockOn)
	{
		ClearLockOn();
		return;
	}

	// プレイヤーの座標
	Vector3 playerPos = player->GetPos();
	// 敵の座標
	Vector3 targetPos = lockOn->GetPos();

	// プレイヤーからターゲットに向かうベクトル(Y方向は0とする)
	Vector3 toTarget = { targetPos.x - playerPos.x, 0.0f, targetPos.z - playerPos.z };

	// 平面上の距離を取得
	float separation = toTarget.Length();

	// 方向ベクトルとする
	toTarget.Normalize();

	float directYaw = atan2f(-toTarget.x, toTarget.z);

	m_targetYaw = directYaw + kLockOnYawOffset;

	// 高さの差を求める
	float heightDiff = targetPos.y - playerPos.y;

	// 高さの差分に併せてピッチも変更する
	m_targetPitch = std::clamp(kLockOnBasePitch + heightDiff * kLockOnPitchFactor, kPitchMin, kPitchMax);

	float halfFovRad = MyLib::ToRadian((GetFovDegree() * 0.5f) - kLockOnFovMargin);
	float horizontalDist = separation * 0.5f / tanf(halfFovRad);

	float requiredDist = horizontalDist / cosf(m_targetPitch);

	m_targetDistance = std::clamp(requiredDist, kLockOnMinDistance, kLockOnMaxDistance);

	float disRate = 1.0f - (requiredDist / kLockOnDisRateMax);

	disRate = std::clamp(disRate, 0.0f, 1.0f);

	// プレイヤーの正面方向を取得
	Vector3 playerForward = -toTarget;

	Vector3 playerRight = Vector3::Cross(Vector3::Up(), toTarget);
	playerRight.Normalize();

	Vector3 localOffset = playerRight * kLockOnOffset.x + Vector3::Up() * kLockOnOffset.y + playerForward * (kLockOnOffset.z + kLockOnOffsetDist * disRate);

	Quaternion offsetRot = Quaternion::AngleAxis(m_lockOnYawOffset, Vector3::Up()) * Quaternion::AngleAxis(m_lockOnPitchOffset, playerRight);

	m_targetEye = playerPos + offsetRot * localOffset;
	
	//m_targetYaw += m_lockOnYawOffset;
	//m_targetPitch = std::clamp(m_targetPitch + m_lockOnPitchOffset, kPitchMin, kPitchMax);
}
