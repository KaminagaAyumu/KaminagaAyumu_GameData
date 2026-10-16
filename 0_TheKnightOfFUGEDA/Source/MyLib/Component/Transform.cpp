#include "Transform.h"
#include "../GameObject.h"

MyLib::Transform::Transform() : 
	m_pos{},
	m_scale{Vector3::One()},
	m_rotation{Quaternion::Identity()}
{
}

MyLib::Transform::~Transform()
{
}

const Matrix4x4 MyLib::Transform::GetWorldMatrix()
{
	// 移動
	Matrix4x4 translationMatrix = Matrix4x4::GetTranslate(m_pos);

	// 回転
	Matrix4x4 rotationMatrix = m_rotation.ToMatrix4x4();

	// 拡大縮小
	Matrix4x4 scaleMatrix = Matrix4x4::GetScale(m_scale);

	// 行列を計算
	Matrix4x4 worldMatrix = translationMatrix * rotationMatrix * scaleMatrix;

	return worldMatrix;
}

const Matrix4x4 MyLib::Transform::GetPivotMatrix()
{
	// 移動
	Matrix4x4 translationMatrix = Matrix4x4::GetTranslate(m_pos);

	// 回転
	Matrix4x4 rotationMatrix = m_rotation.ToMatrix4x4();

	// 拡大縮小
	Matrix4x4 scaleMatrix = Matrix4x4::GetScale(m_scale);

	// 行列を計算
	Matrix4x4 pivotMatrix = rotationMatrix * translationMatrix * scaleMatrix;

	return pivotMatrix;
}

void MyLib::Transform::Init(std::weak_ptr<MyLib::GameObject> parent)
{
}

void MyLib::Transform::Start()
{
}

void MyLib::Transform::Update()
{
}

void MyLib::Transform::End()
{
}
