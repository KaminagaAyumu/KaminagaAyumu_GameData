#include "../Geometry/Vector3.h"
#include <memory>
#include "Model.h"
#include "DxLib.h"
#include "Animation.h"

Model::Model() : 
	m_modelHandle(-1),
	m_isEnable(true)
{

}

Model::~Model()
{
}

void Model::Init()
{
	m_pAnimation = std::make_shared<Animation>();
	m_pAnimation->Init();
	m_isEnable = true;
}

void Model::End()
{
	MV1DeleteModel(m_modelHandle);
	m_pAnimation->End();
}

void Model::Update()
{
	m_pAnimation->Update(m_modelHandle);
}

void Model::Draw()
{
	// 表示状態ならモデルを描画する
	if (m_isEnable)
	{
		MV1DrawModel(m_modelHandle);
	}
}

void Model::SetPos(const Vector3& pos)
{
	MV1SetPosition(m_modelHandle, pos.ToDxLibVector());
}

void Model::SetDir(const Vector3& dir)
{
	MV1SetRotationYUseDir(m_modelHandle, dir.ToDxLibVector(), 0.0f);
}

void Model::SetScale(const Vector3& scale)
{
	MV1SetScale(m_modelHandle, scale.ToDxLibVector());
}

void Model::SetAnimation(int animNo, float animSpeed, int blendTime, bool isLoop)
{
	m_pAnimation->ChangeAnimation(m_modelHandle, animNo, animSpeed, blendTime, isLoop);
}

bool Model::GetAnimationEnd() const
{
	return m_pAnimation->GetAnimEnd();
}
