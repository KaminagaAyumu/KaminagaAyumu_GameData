#pragma once

/// <summary>
/// アニメーション制御クラス
/// </summary>
class Animation
{
public:

	Animation();
	virtual ~Animation();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init();

	/// <summary>
	/// 終了処理
	/// </summary>
	void End();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(int modelHandle);

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
	/// <param name="blendTime">アニメーションを変更するまでの時間</param>
	/// <param name="isLoop">ループするかどうか</param>
	void ChangeAnimation(int modelHandle, int animIndex, float animSpeed, int blendTime, bool isLoop);

	/// <summary>
	/// アニメーションが終わったかどうかを取得
	/// </summary>
	/// <returns></returns>
	bool GetAnimEnd()const { return m_isAnimEnd; }

private:
	// 現在のアニメーションハンドル
	int m_currentAnimHandle;
	// ひとつ前のアニメーションハンドル
	int m_lastAnimHandle;

	// アニメーションをブレンドする時間
	int m_animBlendTime;

	// アニメーションをブレンドするカウント
	int m_animBlendCount;

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

	// 更新処理用の関数ポインタの名称を定義
	using UpdateFunc_t = void (Animation::*)(int);
	// ブレンド処理に使う関数ポインタ
	UpdateFunc_t m_blendUpdate;
	// アニメーションの更新処理に使う関数ポインタ
	UpdateFunc_t m_animUpdate;

private:

	// アニメーションの更新処理
	void SingleUpdate(int modelHandle);
	void LoopUpdate(int modelHandle);

	// アニメーションブレンド関連の更新処理
	void NormalUpdate(int modelHandle);
	void BlendUpdate(int modelHandle);


};