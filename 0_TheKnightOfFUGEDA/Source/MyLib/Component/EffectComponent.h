#pragma once
#include "Component.h"
#include "../../Geometry/Vector3.h"
#include "../../Geometry/Quaternion.h"
#include <memory>
#include <string>
#include <list>
#include <functional>

class Effect;

namespace MyLib
{

	class Transform;

	class EffectComponent : public Component
	{
	public:

		EffectComponent();
		virtual ~EffectComponent();

		void Init(std::weak_ptr<GameObject> parent) override;
		void Start() override;
		void Update() override;
		void End() override;

		using AnchorFunc = std::function<Matrix4x4()>;

		std::weak_ptr<Effect> PlayEffect(const std::wstring& path, const Vector3& offset = Vector3::Zero(), const Quaternion& rotation = Quaternion::Identity());
		
		std::weak_ptr<Effect> PlayEffectWithAnchor(const std::wstring& path, AnchorFunc anchorFunc, const Vector3& offset = Vector3::Zero(), const Quaternion& rotation = Quaternion::Identity());
		
		std::weak_ptr<Effect> PlayOneShotEffect(const std::wstring& path, const Vector3& offset = Vector3::Zero(), const Quaternion& rotation = Quaternion::Identity());
		
		std::weak_ptr<Effect> PlayOneShotEffectWithAnchor(const std::wstring& path, AnchorFunc anchorFunc, const Vector3& offset = Vector3::Zero(), const Quaternion& rotation = Quaternion::Identity());
		
		void StopEffect(std::weak_ptr<Effect> effect);
		void StopAll();

	private:

		std::weak_ptr<Transform> m_pTransform;

		struct PlayingEffect
		{
			std::weak_ptr<Effect> pEffect;
			Vector3 offsetPos;
			Quaternion offsetRotation;
			AnchorFunc anchorFunc;
		};

		std::list<PlayingEffect> m_playingEffects;
	};
}

