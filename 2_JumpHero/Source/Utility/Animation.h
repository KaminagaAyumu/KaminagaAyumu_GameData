#pragma once
#include "Geometry.h"

/// <summary>
/// アニメーション用クラス
/// </summary>
class Animation
{
public:
	/// <summary>
	/// コンストラクタ
	/// 何もアニメーションが入っていない状態で初期化される
	/// </summary>
	Animation();
	virtual ~Animation() = default;


	/// <summary>
	/// アニメーションを設定する(画像が1行しかない場合)
	/// </summary>
	/// <param name="handle">アニメーションさせたい画像</param>
	/// <param name="animSize">表示させたいサイズ</param>
	/// <param name="animNum">表示するアニメーションが何枚あるか(1から数える)</param>
	/// <param name="animFrame">アニメーションが動くフレーム数</param>
	/// <param name="isLoop">ループするかどうか</param>
	void SetAnimation(int handle, const Size& animSize, int animNum, int animFrame, bool isLoop);
	
	/// <summary>
	/// アニメーションを設定する
	/// </summary>
	/// <param name="handle"></param>
	/// <param name="animSize"></param>
	/// <param name="animIndex"></param>
	/// <param name="animNum"></param>
	/// <param name="animFrame"></param>
	/// <param name="isLoop"></param>
	void SetAnimation(int handle, const Size& animSize, int animIndex, int animNum, int animFrame, bool isLoop);

	/// <summary>
	/// アニメーションの更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// アニメーションの描画処理
	/// </summary>
	/// <param name="pos">表示座標</param>
	/// <param name="isTurn">画像の向きを反転するか</param>
	void Draw(Position2Int pos, bool isTurn) const;

	// 大きさや角度を変えたい際にはこの関数を使う
	void SetScale(float scale) { m_animScale = scale; } // 拡大率を設定する
	void SetAngle(float angle) { m_animAngle = angle; } // 回転角度を設定する

	/// <summary>
	/// ループしないアニメーションが終わったかどうかを判定する関数
	/// </summary>
	/// <returns>true : 終わっている false : 終わっていない</returns>
	bool IsEnd()const { return m_isEnd; }

	// アニメーションの変更を検出するために使用する
	bool operator!=(const Animation& anim) const;

private:
	// アニメーションに必須の変数群
	int m_handle; // アニメーションさせたい画像のハンドル
	Size m_animSize; // アニメーションさせたい画像のサイズ
	int m_animNum; // アニメーションさせたい画像の分割数
	int m_animFrame; // アニメーションさせたい画像のフレーム数
	int m_animIndex; // アニメーションさせたい画像の番号(Y座標)
	bool m_isLoop; // アニメーションをループさせるかどうか
	bool m_isEnd; // アニメーションが終わったかどうか

	// アニメーションに必須でない変数群
	float m_animScale; // アニメーションの拡大率
	float m_animAngle; // アニメーションの回転角度

	// アニメーション管理用変数群(このクラス内で完結する)
	int m_frameCount; // フレームカウント
	int m_animCount; // アニメーションカウント(現在アニメーションの何枚目か)
};

