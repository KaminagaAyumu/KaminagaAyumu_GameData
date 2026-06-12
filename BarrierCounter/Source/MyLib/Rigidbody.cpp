#include "Rigidbody.h"

using namespace MyLib;

Rigidbody::Rigidbody() : 
	m_pos{},
	m_dir{},
	m_velocity{}
{
}

void Rigidbody::Init()
{
	m_pos = { 0.0f,0.0f,0.0f };
	m_dir = { 0.0f,0.0f,0.0f };
	m_velocity = { 0.0f,0.0f,0.0f };
}

void Rigidbody::SetVelocity(const Vector3& set)
{
	// 速度ベクトルをセットする
	m_velocity = set;

	// 向きを速度ベクトルを正規化したものにする
	m_dir = m_velocity.Normalized();
}

