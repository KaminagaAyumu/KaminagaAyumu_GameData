#include "Model.h"
#include "../Geometry/Vector3.h"
#include "../Geometry/Matrix4x4.h"
#include "../MyLib/MyMath.h"

Model::Model() : 
	m_modelHandle(-1),
	m_isEnable(true),
	m_slot(Model::ModelSlot::Main),
	m_modelLayer(0),
	m_localOffset{}
{
}

Model::~Model()
{
}

void Model::Init()
{
	m_isEnable = true;
}

void Model::End()
{
	MV1DeleteModel(m_modelHandle);
}

void Model::Update()
{
}

void Model::Draw(const Matrix4x4& worldMatrix) const
{
	// モデルにワールド行列をセットする
	MV1SetMatrix(m_modelHandle, worldMatrix);

	// 表示状態ならモデルを描画する
	if (m_isEnable)
	{
		MV1DrawModel(m_modelHandle);
	}
}

