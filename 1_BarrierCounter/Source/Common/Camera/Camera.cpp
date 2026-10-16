#include "../../Geometry/Vector3.h"
#include "Camera.h"
#include "DxLib.h"
#include "../Input.h"
#include "../Game.h"
#include "../../Geometry/Matrix4x4.h"
#include "../../Geometry/Quaternion.h"
#include <algorithm>

namespace
{
	// 視野角(度数)
	constexpr float kFovDegree = 60.0f;
	// 半円の角度(視野角をラジアンに変換する際に使う)
	constexpr float kHalfCircleDegree = 180.0f;

	// Near,far
	constexpr float kCameraNear = 8.0f;
	constexpr float kCameraFar = 14400.0f;

	// カメラの回転速度
	constexpr float kRotSpeed = 0.015f;
	constexpr float kRotSpeedY = 0.05f;

	// 初期位置とターゲット
	Vector3 kCameraFirstPos = { 0.0f, 300.0f,-600.0f };
	const Vector3 kCameraFirstTarget = { 0.0f,0.0f,0.0f };
	Vector3 kFirstTargetDir = kCameraFirstTarget - kCameraFirstPos;

	// ターゲットとの距離の倍率
	constexpr float kDefaultTargetDistanceRate = 1.5f;
	constexpr float kLongTargetDistanceRate = 2.0f;

	// カメラのY方向の向きの制限
	constexpr float kCameraMaxDirY = 0.2f;
	constexpr float kCameraMinDirY = -0.8f;
}

Camera::Camera() : 
	m_angle(0.0f),
	m_targetDisRate(1.0f),
	m_pos{},
	m_lerpPos{},
	m_targetLocalDir{},
	m_targetDistance(0.0f),
	m_state(CameraState::Default),
	m_shakeFrameCount(0),
	m_shakePower(0.0f)
{
}

Camera::~Camera()
{
}

void Camera::Init()
{
	
	// 視野角の設定
	float fovRadian = kFovDegree * (DX_PI_F / kHalfCircleDegree);
	SetupCamera_Perspective(fovRadian);
	SetCameraNearFar(kCameraNear, kCameraFar);

	// カメラの設定
	SetCameraPositionAndTarget_UpVecY(kCameraFirstPos.ToDxLibVector(), kCameraFirstTarget.ToDxLibVector());

	// 座標の初期化
	m_pos = kCameraFirstPos;
	m_lerpPos = m_pos;

	// 初期ターゲットの方向を正規化(名前空間内ではできないためここで行う)
	kFirstTargetDir.Normalize();

	// カメラとターゲットの距離を取得
	m_targetDistance = Vector3::GetDistance(kCameraFirstPos, kCameraFirstTarget);

	//ターゲットの方向の初期化
	m_targetLocalDir = kFirstTargetDir;

	// ターゲットの方向を正規化
	m_targetLocalDir.Normalize();

	// ターゲットとの距離の割合を初期化
	m_targetDisRate = kDefaultTargetDistanceRate;
}

void Camera::Update(Vector3 target, float fovDegree)
{
	// スティックのデータを取得
	auto stick = Input::GetInstance().GetStickData();

	// 右スティックの左右が入力されているかを取得
	bool isOnRightStickSide = stick.rightStick.x != 0;
	// 右スティックの上下が入力されているかを取得
	bool isOnRightStickForward = stick.rightStick.y != 0;
	
	// 右スティックの左右が入力されているとき
	if (isOnRightStickSide)
	{
		// 右スティックのX方向への移動量をY軸回転に変換
		m_angle = stick.rightStick.x * kRotSpeed * DX_PI_F;

		// 回転軸を作成
		Vector3 axis = Vector3::Up();

		// 回転クオータニオンを作成
		Quaternion rotQ;

		// 軸を基準とした回転クオータニオンを作成
		rotQ = rotQ.AngleAxis(m_angle, axis);

		// クオータニオンの値をデバッグ表示
		rotQ.DrawAxisLine(Vector3::Zero(), rotQ, 1.0f);

		// クオータニオンで回転後の方向ベクトルを作成
		// (方向ベクトルを回転させているので方向ベクトルが出る)
		m_targetLocalDir = rotQ * m_targetLocalDir;
	}

	// 右スティックの上下が入力されているとき
	if (isOnRightStickForward)
	{
		// 現在のローカル方向ベクトルを取得し、未来の方向とする
		Vector3 nextDir = m_targetLocalDir;

		// 方向ベクトルのY座標を直接いい感じにいじる
		nextDir.y += stick.rightStick.y * kRotSpeedY;
		// 正規化
		nextDir.Normalize();

		// いい感じに上下を制限
		if (nextDir.y <= kCameraMaxDirY && nextDir.y >= kCameraMinDirY)
		{
			// 制限範囲内なら実際にY座標を変更
			m_targetLocalDir = nextDir;
		}

	}
	
	// カメラ変更ボタンが押されたら
	if (Input::GetInstance().IsTriggered("CameraChange"))
	{
		// カメラの状態によってカメラの向きを変える
		if (m_state == CameraState::Default)
		{
			// 遠いカメラに変更
			m_state = CameraState::Long;
			m_targetDisRate = kLongTargetDistanceRate;
		}
		else
		{
			// デフォルトのカメラに変更
			m_state = CameraState::Default;
			m_targetDisRate = kDefaultTargetDistanceRate;
		}
	}

	/*
	// 入力値の大きさが0でない場合
	if (axis.Length() != 0.0f)
	{
		// 回転クオータニオンを別々にした後合成する
		Quaternion rotY;
		Quaternion rotX;
		Quaternion rotation;

		// 右スティックの左右が入力されているとき
		if (isOnRightStickSide)
		{
			// 右スティックのX方向への移動量をY軸回転に変換
			float angleY = stick.rightStick.x * kRotSpeed * DX_PI_F;

			// 回転軸を作成
			Vector3 axis = Vector3::Up();

			// 軸を基準とした回転クオータニオンを作成
			rotY = rotY.AngleAxis(angleY, axis);

			// クオータニオンの値をデバッグ表示
			rotY.DrawAxisLine(Vector3::Zero(), rotY, 1.0f);
		}

		// 右スティックの上下が入力されているとき
		if (isOnRightStickForward)
		{
			// 右スティックのY方向への移動量をX軸回転に変換
			float angleX = stick.rightStick.y * kRotSpeed * DX_PI_F;

			// 回転軸を作成
			Vector3 axis = Vector3::Right();

			// 軸を基準とした回転クオータニオンを作成
			rotX = rotX.AngleAxis(angleX, axis);

			// クオータニオンの値をデバッグ表示
			rotX.DrawAxisLine(Vector3::Zero(), rotX, 1.0f);
		}

		rotation = rotY * rotX;

		// クオータニオンで回転後の方向ベクトルを作成
		// (方向ベクトルを回転させているので方向ベクトルが出る)
		m_targetLocalDir = rotation * m_targetLocalDir;
	}
	*/

	/*ターゲットとの距離を一定にする*/
	//1.ターゲットへの方向ベクトルに距離をかけて、差分ベクトルを作成
	Vector3 targetDel = m_targetLocalDir * m_targetDistance * m_targetDisRate;
	//2.差分ベクトルを足して、距離を一定に保つ
	m_pos = target - targetDel;

	// 視野角の設定
	float fovRadian = fovDegree * (DX_PI_F / 180.0f);
	SetupCamera_Perspective(fovRadian);
	SetCameraNearFar(kCameraNear, kCameraFar);

	// カメラが揺れる状態の時
	if (m_shakeFrameCount > 0)
	{
		// カメラを揺らす処理を行う
		ShakeUpdate();
	}

	// カメラの設定
	SetCameraPositionAndTarget_UpVecY(m_pos.ToDxLibVector(), target.ToDxLibVector());
	//SetCameraPositionAndTargetAndUpVec(m_pos.ToDxLibVector(), target.ToDxLibVector(), Vector3::Up().ToDxLibVector());
}

void Camera::UpdateNoRotation(Vector3 target, float fovDegree)
{
	/*ターゲットとの距離を一定にする*/
	//1.ターゲットへの方向ベクトルに距離をかけて、差分ベクトルを作成
	Vector3 targetDel = m_targetLocalDir * m_targetDistance * m_targetDisRate;
	//2.差分ベクトルを足して、距離を一定に保つ
	m_pos = target - targetDel;

	// 視野角の設定
	float fovRadian = fovDegree * (DX_PI_F / 180.0f);
	SetupCamera_Perspective(fovRadian);
	SetCameraNearFar(kCameraNear, kCameraFar);

	// カメラの設定
	SetCameraPositionAndTarget_UpVecY(m_pos.ToDxLibVector(), target.ToDxLibVector());
}

void Camera::ShakeUpdate()
{
	// 揺れるフレーム数を減らす
	m_shakeFrameCount--;

	// カメラの位置にランダムな値を加える
	m_pos.x += (float)(rand() % 100 - 50) / 100.0f * m_shakePower;
	m_pos.y += (float)(rand() % 100 - 50) / 100.0f * m_shakePower;
	m_pos.z += (float)(rand() % 100 - 50) / 100.0f * m_shakePower;

	// 揺れるフレーム数が0以下になったら揺れを止める
	if (m_shakeFrameCount <= 0)
	{
		m_shakeFrameCount = 0;
		m_shakePower = 0.0f;
	}
}

void Camera::Draw()
{

}

void Camera::LerpUpdate(Vector3 from, Vector3 to, float speed)
{
	Vector3 currentTarget = Vector3::LerpVec3(from, to, speed);

	/*ターゲットとの距離を一定にする*/
	//1.ターゲットへの方向ベクトルに距離をかけて、差分ベクトルを作成
	Vector3 targetDel = m_targetLocalDir * m_targetDistance * m_targetDisRate;
	//2.差分ベクトルを足して、距離を一定に保つ
	m_pos = currentTarget - targetDel;

	// 視野角の設定
	float fovRadian = kFovDegree * (DX_PI_F / 180.0f);
	SetupCamera_Perspective(fovRadian);
	SetCameraNearFar(kCameraNear, kCameraFar);

	// カメラの設定
	SetCameraPositionAndTarget_UpVecY(m_pos.ToDxLibVector(), currentTarget.ToDxLibVector());
	//SetCameraPositionAndTargetAndUpVec(m_pos.ToDxLibVector(), currentTarget.ToDxLibVector(), Vector3{0.0f,0.0f,1.0f}.ToDxLibVector());

}

bool Camera::IsInCamera(const Vector3& target, float radius) const
{
	/*
	//// カメラから対象へのベクトルを作成
	//Vector3 cameraToTarget = target - m_pos;

	//// カメラから対象への距離を取得
	//float dist = cameraToTarget.Length();

	//// 距離がNear,farの範囲外ならばfalseとする
	//if (dist < kCameraNear - radius || dist > kCameraFar + radius)
	//{
	//	return false;
	//}

	//// カメラから対象への向きを取得
	//Vector3 dir = cameraToTarget.Normalized();

	//// カメラの向きと対象への向きの内積を取得
	//float dot = Vector3::Dot(m_targetLocalDir, dir);

	//// 視野の半分を取得
	//float halfFov = kFovDegree * 0.5f * (DX_PI_F / 180.0f);

	//// 角度からの範囲を取得
	//float angleMargin = 0.0f;
	//if (radius > 0.0f && dist > radius)
	//{
	//	angleMargin = asinf(radius / dist);
	//}
	//
	//// 
	//return dot >= cosf(halfFov + angleMargin);
	*/

	// カメラのビュー行列を取得
	MATRIX view = GetCameraViewMatrix();

	// ワールド座標をビュー座標に変換
	VECTOR v = VTransform(target.ToDxLibVector(), view);

	// Zが前方でないならカメラの範囲外
	if (v.z <= 0) return false;

	// 視野角のラジアンを取得
	float fovRadian = kFovDegree * (DX_PI_F / 180.0f);

	// 視野角のtanの半分を取得
	float tanHalfFov = tanf(fovRadian / 2.0f);

	if (fabs(v.y / v.z) > tanHalfFov) return false;

	float aspect = (float)Game::kScreenWidth / Game::kScreenHeight;
	if (fabs(v.x / v.z) > tanHalfFov * aspect) return false;

	return true;
}

void Camera::ShakeCamera(float power, int shakeFrame)
{
	// カメラがすでに揺れている状態ならば揺らす処理を行わない
	if (m_shakeFrameCount > 0)
	{
		return;
	}
	m_shakeFrameCount = shakeFrame;
	m_shakePower = power;
}
