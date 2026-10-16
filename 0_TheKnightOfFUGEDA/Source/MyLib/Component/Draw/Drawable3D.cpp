#include "Drawable3D.h"
#include "../../Renderer.h"
#include "../../../Common/Model.h"
#include "../../GameObject.h"
#include <cassert>

MyLib::Drawable3D::Drawable3D(DrawLayer layer) : 
	Drawable(layer),
	m_isCastShadow(true),
	m_isReceiveShadow(false)
{
}

void MyLib::Drawable3D::Init(std::weak_ptr<MyLib::GameObject> parent)
{
	// 親となるゲームオブジェクトを取得する
	std::shared_ptr<MyLib::GameObject> pParent = parent.lock();
	// 親からTransformコンポーネントの参照を得る
	m_pTransform = pParent->GetComponent<MyLib::Transform>();
	// Transformが親にない場合assertする
	if (!m_pTransform.lock())
	{
		assert(false && "Drawable3D : Transformコンポーネントがありません");
	}
	Renderer::GetInstance().Entry(shared_from_this());
}

void MyLib::Drawable3D::Start()
{
}

std::shared_ptr<Model> MyLib::Drawable3D::AddModel(Model::ModelSlot slot, int handle)
{
	std::shared_ptr<Model> pModel = std::make_shared<Model>();
	pModel->SetModelHandle(MV1DuplicateModel(handle));
	pModel->SetSlot(slot);
	pModel->Init();
	m_pModels.push_back(pModel);

	return pModel;
}

std::shared_ptr<Model> MyLib::Drawable3D::GetModel(Model::ModelSlot slot)
{
	for (std::shared_ptr<Model> model : m_pModels)
	{
		if (model->GetSlot() == slot)
		{
			return model;
		}
	}
	return std::shared_ptr<Model>();
}

const int MyLib::Drawable3D::GetModelHandle(Model::ModelSlot slot) const
{
	for (std::shared_ptr<Model> model : m_pModels)
	{
		if (model->GetSlot() == slot)
		{
			return model->GetModelHandle();
		}
	}
	return -1;
}

void MyLib::Drawable3D::SetEnable(Model::ModelSlot slot, bool isEnable)
{
	std::shared_ptr<Model> pModel = GetModel(slot);
	// モデルが取得できた場合表示状態を変更する
	if (pModel)
	{
		GetModel(slot)->SetEnable(isEnable);
	}
}

bool MyLib::Drawable3D::IsEnable(Model::ModelSlot slot)
{
	std::shared_ptr<Model> pModel = GetModel(slot);
	// モデルが取得できた場合表示状態を取得する
	if (pModel)
	{
		return GetModel(slot)->IsEnable();
	}
	// モデルが取得できない場合falseとする
	return false;
}

void MyLib::Drawable3D::SetModelOffset(Model::ModelSlot slot, std::weak_ptr<MyLib::Transform> offset)
{
	std::shared_ptr<Model> pModel = GetModel(slot);
	// モデルが取得できた場合モデルのオフセットを変更する
	if (pModel)
	{
		GetModel(slot)->SetLocalOffset(offset);
	}
}

void MyLib::Drawable3D::SetAnchor(Model::ModelSlot slot, Model::AnchorFunc func)
{
	std::shared_ptr<Model> pModel = GetModel(slot);
	// モデルが取得できた場合表示状態を変更する
	if (pModel)
	{
		GetModel(slot)->SetAnchor(func);
	}
}

void MyLib::Drawable3D::End()
{
	// まとめて終了処理を行う
	for (std::shared_ptr<Model> model : m_pModels)
	{
		model->End();
	}

	Renderer::GetInstance().Exit(shared_from_this());
}

void MyLib::Drawable3D::Update()
{
}

void MyLib::Drawable3D::Draw() const
{
	for (const auto& model : m_pModels)
	{
		model->Draw(GetLocalMatrix(model));
	}
}

float MyLib::Drawable3D::GetDepth()
{
	// TODO:depthを変数で持つかどうかを決める
	return 0.0f;
}

Matrix4x4 MyLib::Drawable3D::GetLocalMatrix(std::shared_ptr<Model> model) const
{
	// 現在位置を取得
	std::shared_ptr<Transform> pTransform = m_pTransform.lock();

	std::shared_ptr<Transform> pLocalTransform = model->GetLocalOffset().lock();
	bool isUseLocalOffset = pLocalTransform != nullptr;

	// ローカルオフセットを使用する場合
	if (isUseLocalOffset)
	{
		// アンカーを使用する場合
		if (model->IsUseAnchor())
		{
			return model->GetAnchorMatrix() * pLocalTransform->GetPivotMatrix();
		}
		else // アンカーを使用しない場合
		{
			return pLocalTransform->GetPivotMatrix() * pTransform->GetWorldMatrix();
		}
	}
	else // ローカルオフセットを使用しない場合
	{
		// アンカーを使用する場合
		if (model->IsUseAnchor())
		{
			return model->GetAnchorMatrix();
		}
		else // アンカーを使用しない場合
		{
			return pTransform->GetWorldMatrix();
		}
	}
}
