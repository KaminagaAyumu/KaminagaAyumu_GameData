#pragma once
#include "../../Component.h"
#include "../../../State/StateMachine.h"

namespace MyLib
{
	class GameObject;
	class Transform;
	class Animator;
	/// <summary>
	/// 敵の挙動を制御するコンポーネントの基底
	/// </summary>
	class EnemyController : public Component
	{
	public:
		EnemyController();
		virtual ~EnemyController();

		std::weak_ptr<GameObject> GetOwnerObj() { return m_pOwnerObj; }

		std::weak_ptr<Animator> GetAnimator() { return m_pAnimator; }

		/// <summary>
		/// プレイヤーの位置をセットする
		/// (プレイヤーを見るため)
		/// </summary>
		/// <param name="player">プレイヤーの位置</param>
		void SetPlayer(std::weak_ptr<GameObject> player);

		std::weak_ptr<Transform> GetPlayerPos() const;

		void SetCanAct(bool canAct);

		bool TryNotifyDetect();

		bool ResetDetectNotify();

	protected:
		std::weak_ptr<GameObject> m_pOwnerObj;	// 親となるゲームオブジェクト

		std::weak_ptr<Animator> m_pAnimator;	// Animatorコンポーネント

		std::weak_ptr<GameObject> m_pPlayer;			// プレイヤーオブジェクト

		// 敵の状態を管理するステートマシン
		StateMachine<EnemyController> m_stateMachine;

		// 発見がすでに行われたかどうか
		bool m_isDetectNotified;
	};
}


