#pragma once
#include "../../Component.h"
#include "../../../State/StateMachine.h"
#include "../../../../Utility/CSV/PlayerAttackResource.h"
#include <string>
#include <map>

namespace MyLib
{
	class Transform;
	class Drawable3D;
	class Rigidbody;
	class Animator;
	class SphereCollider;
	class BoxCollider;
	class EffectComponent;
	/// <summary>
	/// プレイヤー挙動を制御するコンポーネント
	/// </summary>
	class PlayerController : public Component
	{
	public:

		PlayerController();
		virtual ~PlayerController();

		void Init(std::weak_ptr<MyLib::GameObject> parent) override;
		void Start() override;
		void Update() override;
		void End() override;

		void SetCanAct(bool canAct);

		void SetLockOnTarget(std::weak_ptr<MyLib::Transform> target) { m_pLockOnTarget = target; }

		void ClearLockOnTarget() { m_pLockOnTarget.reset(); }

		bool IsBuffered(const std::string& name) const;

		void ClearBuffer(const std::string& name);

		bool IsJustGuard();

		bool IsPlayerDie();

		bool IsLockOn();

		/// <summary>
		/// test
		/// </summary>
		/// <returns></returns>
		std::weak_ptr<Transform> GetTransform() { return m_pTransform; }

		std::weak_ptr<Rigidbody> GetRigidbody() { return m_pRigidbody; }

		std::weak_ptr<Animator> GetAnimator() { return m_pAnimator; }

		std::weak_ptr<Drawable3D> GetDrawable3D() { return m_pDrawable3D; }

		std::weak_ptr<SphereCollider> GetAttackCollider() { return m_pAttackCollider; }

		std::weak_ptr<SphereCollider> GetGuardCollider() { return m_pGuardCollider; }

		std::weak_ptr<EffectComponent> GetEffectComponent() { return m_pEffectComponent; }

		const PlayerAttackResource& GetAttackResource() { return m_attackResource; }

		int GetMaxLife();

		int GetLife() { return m_hp; }

	private:
		std::weak_ptr<MyLib::Transform> m_pTransform;	// Transformコンポーネント(test)

		std::weak_ptr<MyLib::Transform> m_pLockOnTarget;	// ロックオンの対象

		std::shared_ptr<MyLib::Transform> m_pModelOffset;	// プレイヤーのモデルのオフセット
		
		std::shared_ptr<MyLib::Transform> m_pSwordOffset;	// プレイヤーの剣のオフセット

		std::shared_ptr<MyLib::Transform> m_pShieldOffset;	// プレイヤーの盾のオフセット

		std::weak_ptr<Drawable3D> m_pDrawable3D;	// Drawable3Dコンポーネント

		std::weak_ptr<Rigidbody> m_pRigidbody;	// Rigidbodyコンポーネント

		std::weak_ptr<Animator> m_pAnimator;	// Animatorコンポーネント

		std::weak_ptr<SphereCollider> m_pAttackCollider;

		std::weak_ptr<SphereCollider> m_pGuardCollider;

		std::weak_ptr<EffectComponent> m_pEffectComponent;	// エフェクトコンポーネント

		// プレイヤーの状態を管理するステートマシン
		StateMachine<PlayerController> m_stateMachine;

		PlayerAttackResource m_attackResource;	// 攻撃データを管理するクラス

		int m_invincibleFrame;	// 無敵時間

		int m_guardingFrame;	// 盾を構えているフレーム数

		int m_hp;				// HP

		std::map<std::string, int> m_inputBufferFrames; // 入力を保持するmap

		bool m_isGuard;	// ガード状態かどうか

	private:
		void UpdateInputBuffer();

		/// <summary>
		/// 無敵かどうか
		/// </summary>
		/// <returns></returns>
		bool IsInvincible() { return m_invincibleFrame > 0; }

		/// <summary>
		/// ガードしているかどうかの判定を行う
		/// </summary>
		/// <returns></returns>
		bool IsGuarding();
	};
}



