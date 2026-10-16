#include "EffectComponent.h"
#include "Transform.h"
#include "../GameObject.h"
#include "../../Common/Effect/EffectManager.h"
#include "../../Common/Effect/Effect.h"
#include <cassert>

MyLib::EffectComponent::EffectComponent()
{
}

MyLib::EffectComponent::~EffectComponent()
{
}

void MyLib::EffectComponent::Init(std::weak_ptr<GameObject> parent)
{
	std::shared_ptr<MyLib::GameObject> pParent = parent.lock();
	// 親からTransformコンポーネントの参照を得る
	m_pTransform = pParent->GetComponent<MyLib::Transform>();
	// Transformが親にない場合assertする
	if (!m_pTransform.lock())
	{
		assert(false && "EffectComponent : Transformコンポーネントがありません");
	}
}

void MyLib::EffectComponent::Start()
{
}

void MyLib::EffectComponent::Update()
{
	std::shared_ptr<Transform> pTransform = m_pTransform.lock();

	for(auto& playingEffect : m_playingEffects)
	{
		if(std::shared_ptr<Effect> pEffect = playingEffect.pEffect.lock())
		{
			if (playingEffect.anchorFunc)
			{
				Matrix4x4 anchorMatrix = playingEffect.anchorFunc();
				Vector3 worldOffset = anchorMatrix.GetRotation() * playingEffect.offsetPos;
				pEffect->SetPos(anchorMatrix.GetPosition() + worldOffset);
				pEffect->SetRotation(anchorMatrix.GetRotation() * playingEffect.offsetRotation);
			}
			else
			{
				Vector3 worldOffset = pTransform->GetRotation() * playingEffect.offsetPos;
				pEffect->SetPos(pTransform->GetPos() + worldOffset);
			}
		}
	}

	m_playingEffects.remove_if([](const PlayingEffect effect)
	{
		return effect.pEffect.expired();
	});
}

void MyLib::EffectComponent::End()
{
}

std::weak_ptr<Effect> MyLib::EffectComponent::PlayEffect(const std::wstring& path, const Vector3& offset, const Quaternion& rotation)
{
	std::weak_ptr<Effect> effect = EffectManager::GetInstance().CreateEffect(path, m_pTransform.lock()->GetPos() + offset, rotation);
	m_playingEffects.push_back({ effect, offset, rotation, nullptr });
	return effect;
}

std::weak_ptr<Effect> MyLib::EffectComponent::PlayEffectWithAnchor(const std::wstring& path, AnchorFunc anchorFunc, const Vector3& offset, const Quaternion& rotation)
{
	Matrix4x4 anchorMatrix = anchorFunc();
	Vector3 startPos = anchorMatrix.GetPosition() + anchorMatrix.GetRotation() * offset;

	std::weak_ptr<Effect> effect = EffectManager::GetInstance().CreateEffect(path, startPos, rotation);
	m_playingEffects.push_back({ effect, offset, rotation, std::move(anchorFunc) });

	return effect;
}

std::weak_ptr<Effect> MyLib::EffectComponent::PlayOneShotEffect(const std::wstring& path, const Vector3& offset, const Quaternion& rotation)
{
	return EffectManager::GetInstance().CreateEffect(path, m_pTransform.lock()->GetPos() + offset, rotation);
}

std::weak_ptr<Effect> MyLib::EffectComponent::PlayOneShotEffectWithAnchor(const std::wstring& path, AnchorFunc anchorFunc, const Vector3& offset, const Quaternion& rotation)
{
	Matrix4x4 anchorMatrix = anchorFunc();
	Vector3 startPos = anchorMatrix.GetPosition() + anchorMatrix.GetRotation() * offset;

	std::weak_ptr<Effect> effect = EffectManager::GetInstance().CreateEffect(path, startPos, rotation);
	return effect;
}

void MyLib::EffectComponent::StopEffect(std::weak_ptr<Effect> effect)
{
	if(std::shared_ptr<Effect> pEffect = effect.lock())
	{
		pEffect->StopEffect();
	}
}

void MyLib::EffectComponent::StopAll()
{
	for (auto& effect : m_playingEffects)
	{
		if(std::shared_ptr<Effect> pEffect = effect.pEffect.lock())
		{
			pEffect->StopEffect();
		}
	}
}
