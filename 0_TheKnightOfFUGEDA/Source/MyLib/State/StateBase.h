#pragma once
#include <memory>

namespace MyLib
{
	template<typename OwnerType>
	class StateMachine;

	/// <summary>
	/// ステートの基底クラス
	/// </summary>
	/// <typeparam name="OwnerType">ステートを使用するオーナーの型(Playerなど)</typeparam>
	template<typename OwnerType>
	class StateBase
	{	
	private:
		/// <summary>
		/// 使用するステートマシンを設定する
		/// </summary>
		/// <param name="stateMachine">ステートマシンのポインタ</param>
		void SetStateMachine(StateMachine<OwnerType>* stateMachine) { m_pStateMachine = stateMachine; }

		// 以下の関数はtemplateを使っている関係上ヘッダーで定義しています
		/// <summary>
		/// 初期化処理(ステートマシンから呼び出す用)
		/// </summary>
		/// <param name="owner">使用するオーナーのポインタ</param>
		void Init(OwnerType* owner)
		{
			if (!owner) return;
			// オーナーのポインタをセットする
			m_pOwner = owner;
			OnInit(owner);
		}

		/// <summary>
		/// 更新処理(ステートマシンから呼び出す用)
		/// </summary>
		void Update()
		{
			if (!m_pOwner) return;
			OnUpdate();
		}

		/// <summary>
		/// 終了処理(ステートマシンから呼び出す用)
		/// </summary>
		void End()
		{
			if (!m_pOwner) return;
			OnEnd();
		}

	protected:

		// オーナーの型を設定したステートマシンをフレンドにする
		friend class StateMachine<OwnerType>;

		/// <summary>
		/// 初期化処理(ステート内での管理用)
		/// 使用するオーナーをセットする
		/// </summary>
		/// <param name="owner">使用するオーナーのポインタ</param>
		virtual void OnInit(OwnerType* owner) abstract;
		/// <summary>
		/// 更新処理(ステート内での管理用)
		/// </summary>
		virtual void OnUpdate() abstract;
		/// <summary>
		/// 終了処理(ステート内での管理用)
		/// </summary>
		virtual void OnEnd() abstract;

		OwnerType* m_pOwner;						// ステートを使用するオーナーのポインタ
		StateMachine<OwnerType>* m_pStateMachine;	// ステートマシンのポインタ
	};
}

