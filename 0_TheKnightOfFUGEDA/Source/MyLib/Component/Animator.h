#pragma once
#include "Component.h"
#include <string>

namespace MyLib
{
	/// <summary>
	/// アニメーションを行うコンポーネント
	/// </summary>
	class Animator : public Component
	{
	public:
		Animator();
		virtual ~Animator() = default;

		void Init(std::weak_ptr<MyLib::GameObject> parent) override;
		void Start() override;
		void Update() override;
		void End() override;

		/// <summary>
		/// アニメーションをセットする
		/// </summary>
		/// <param name="modelHandle"></param>
		/// <param name="animIndex"></param>
		void SetAnimation(int modelHandle, int animIndex);

		/// <summary>
		/// アニメーションを変更する
		/// </summary>
		/// <param name="animIndex">アニメーション番号</param>
		/// <param name="animSpeed">アニメーションの速度(割合)</param>
		/// <param name="blendTime">アニメーションを変更するまでの時間</param>
		/// <param name="isLoop">ループするかどうか</param>
		void ChangeAnimation(int animIndex, float animSpeed, float blendFrame, bool isLoop);

		/// <summary>
		/// アニメーションを変更する
		/// </summary>
		/// <param name="animName">アニメーション名</param>
		/// <param name="animSpeed">アニメーションの速度(割合)</param>
		/// <param name="blendTime">アニメーションを変更するまでの時間</param>
		/// <param name="isLoop">ループするかどうか</param>
		void ChangeAnimation(const std::wstring_view& animName, float animSpeed, float blendFrame, bool isLoop);
		

		void ChangeAnimation(const std::wstring& animName, float animSpeed, float blendFrame, bool isLoop);

		int GetCurrentAnimFrame() const { return static_cast<int>(m_currentAnimCount); }

		/// <summary>
		/// アニメーションが終わったかどうかを取得
		/// </summary>
		/// <returns></returns>
		bool GetAnimEnd()const { return m_isAnimEnd; }

	private:

		std::weak_ptr<MyLib::GameObject> m_pParent;

		// アニメーションを行うモデルのハンドル
		int m_modelHandle;

		// 現在のアニメーションハンドル
		int m_currentAnimHandle;
		
		// 現在のアニメーション番号
		int m_currentAnimIndex;
		
		// ひとつ前のアニメーションハンドル
		int m_lastAnimHandle;
		
		// アニメーションをブレンドする時間
		float m_animBlendFrame;

		// アニメーションをブレンドするカウント
		float m_animBlendCount;

		// 現在のアニメーションのカウント
		float m_currentAnimCount;
		// ひとつ前のアニメーションのカウント
		float m_lastAnimCount;

		// 現在のアニメーションのスピード
		float m_currentAnimSpeed;

		// ひとつ前のアニメーションのスピード
		float m_lastAnimSpeed;

		// アニメーションが終わったか
		bool m_isAnimEnd;

		// 更新処理の関数ポインタの名称を定義
		using UpdateFunc_t = void(Animator::*)(int);
		// ブレンド処理に使う関数ポインタ
		UpdateFunc_t m_blendUpdate;
		// 通常のアニメーションの更新に使う関数ポインタ
		UpdateFunc_t m_animUpdate;

	private:

		// アニメーションの更新処理
		void SingleUpdate(int modelHandle);
		void LoopUpdate(int modelHandle);

		void NormalUpdate(int modelHandle);
		void BlendUpdate(int modelHandle);
	};

}



