#pragma once
#include "StateBase.h"
#include <memory>
#include <functional>

namespace MyLib
{
	/// <summary>
	/// 状態遷移を管理するクラス
	/// </summary>
	/// <typeparam name="OwnerType">ステートマシンを使用するオーナーの型(Playerなど)</typeparam>
	template<typename OwnerType>
	class StateMachine
	{
	public:

		// 以下の関数はtemplateを使っている関係上ヘッダーで定義しています

		/// <summary>
		/// コンストラクタ
		/// </summary>
		StateMachine() : m_onStateChangeFunc([](){})
		{
		}
		virtual ~StateMachine() = default;

		/// <summary>
		/// 初期化処理
		/// </summary>
		/// <param name="owner">使用するオーナーのポインタ</param>
		void Init(OwnerType* owner)
		{
			m_pOwner = owner;
			m_onStateChangeFunc = [](){};
		}

		/// <summary>
		/// 更新処理
		/// </summary>
		void Update()
		{
			// 最初にステートの変更処理を行う
			// ステート側で変更時にこのラムダ式の中身が設定されるためその処理が呼ばれる
			m_onStateChangeFunc();

			// 処理が終わったらラムダ式の中身を空にする
			m_onStateChangeFunc = [](){};

			// 現在のステートが存在する場合は更新処理を行う
			if(m_pCurrentState)
			{
				m_pCurrentState->Update();
			}
		}

		/// <summary>
		/// 終了処理
		/// </summary>
		void End()
		{
			// 現在のステートが存在する場合は終了処理を行う
			if (m_pCurrentState)
			{
				m_pCurrentState->End();
			}
			// ステートのポインタを初期化する
			m_pOwner = nullptr;
		}

		/// <summary>
		/// ステートを変更する
		/// </summary>
		/// <typeparam name="StateType">ステートの型</typeparam>
		/// <typeparam name="...Args">ステートのコンストラクタに渡す引数の型</typeparam>
		/// <param name="...args"></param>
		template<typename StateType, typename... Args>
		void ChangeState(Args&&... args)
		{
			// 現在のステートを持っており、変更先のステートが同じなら処理しない
			if (m_pCurrentState && typeid(*m_pCurrentState) == typeid(StateType))
			{
				return;
			}

			// ステートを変更するためにラムダ式を設定する
			m_onStateChangeFunc = [&]()
				{
					// オーナーが存在しない場合は処理を行わない
					if (m_pOwner == nullptr) return;

					// ステートが存在する場合は終了処理を行う
					if (m_pCurrentState)
					{
						m_pCurrentState->End();
						// ステートのポインタを初期化する
						m_pCurrentState.reset();
					}

					// 新しいステートを作成する
					m_pCurrentState = std::make_shared<StateType>(std::forward<Args>(args)...);

					// 新しいステートが存在しない場合は処理を行わない
					if (!m_pCurrentState) return;

					// ステートマシンのポインタをセットする
					m_pCurrentState->SetStateMachine(this);

					// 新しいステートの初期化処理を行う
					m_pCurrentState->Init(m_pOwner);
				};
		}

		/// <summary>
		/// ステートを変更する
		/// 同じステートでも変更できるバージョン
		/// </summary>
		/// <typeparam name="StateType">ステートの型</typeparam>
		/// <typeparam name="...Args">ステートのコンストラクタに渡す引数の型</typeparam>
		/// <param name="...args"></param>
		template<typename StateType, typename... Args>
		void ChangeStateReset(Args&&... args)
		{
			// ステートを変更するためにラムダ式を設定する
			m_onStateChangeFunc = [&]()
				{
					// オーナーが存在しない場合は処理を行わない
					if (m_pOwner == nullptr) return;

					// ステートが存在する場合は終了処理を行う
					if (m_pCurrentState)
					{
						m_pCurrentState->End();
						// ステートのポインタを初期化する
						m_pCurrentState.reset();
					}

					// 新しいステートを作成する
					m_pCurrentState = std::make_shared<StateType>(std::forward<Args>(args)...);

					// 新しいステートが存在しない場合は処理を行わない
					if (!m_pCurrentState) return;

					// ステートマシンのポインタをセットする
					m_pCurrentState->SetStateMachine(this);

					// 新しいステートの初期化処理を行う
					m_pCurrentState->Init(m_pOwner);
				};
		}

		template<typename StateType>
		bool IsCheckState()
		{
			// 現在のステートを持っており、変更先のステートが同じならtrue
			return (m_pCurrentState && typeid(*m_pCurrentState) == typeid(StateType));
		}

	private:

		// 現在のステートのポインタ
		std::shared_ptr<StateBase<OwnerType>> m_pCurrentState;

		// ステートが変更された際に呼ばれる関数
		std::function<void()> m_onStateChangeFunc;

		// ステートマシンを使用するオーナーのポインタ
		OwnerType* m_pOwner;
	};
}



