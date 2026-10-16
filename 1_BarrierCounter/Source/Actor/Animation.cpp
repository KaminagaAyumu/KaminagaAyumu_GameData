#include "Animation.h"
#include "DxLib.h"

Animation::Animation() : 
	m_currentAnimHandle(-1),
	m_lastAnimHandle(-1),
	m_animBlendTime(0),
	m_animBlendCount(0),
	m_currentAnimCount(0.0f),
	m_lastAnimCount(0.0f),
	m_currentAnimSpeed(0.0f),
	m_lastAnimSpeed(0.0f),
	m_isAnimEnd(false),
	m_blendUpdate(&Animation::NormalUpdate),
	m_animUpdate(&Animation::SingleUpdate)
{
}

Animation::~Animation()
{
}

void Animation::Init()
{
	// 更新処理を初期状態のものにする(ループしないアニメーションを行う)
	m_blendUpdate = &Animation::NormalUpdate;
	m_animUpdate = &Animation::SingleUpdate;
}

void Animation::End()
{

}

void Animation::Update(int modelHandle)
{
	// 現在の更新処理を行う
	(this->*m_blendUpdate)(modelHandle);
	(this->*m_animUpdate)(modelHandle);
}

void Animation::SetAnimation(int modelHandle, int animIndex)
{
	// アニメーションのハンドルを取得
	m_currentAnimHandle = MV1AttachAnim(modelHandle, animIndex, -1);
}

void Animation::ChangeAnimation(int modelHandle, int animIndex, float animSpeed, int blendTime, bool isLoop)
{
	// すでにアニメーションブレンド中の場合
	if (m_blendUpdate == &Animation::BlendUpdate)
	{
		// 前回のアニメーションが存在する場合強制的に消去
		if (m_lastAnimHandle != -1)
		{
			// デタッチして消去
			MV1DetachAnim(modelHandle, m_lastAnimHandle);
			m_lastAnimHandle = -1;
		}
		// 通常の更新処理に変更
		m_blendUpdate = &Animation::NormalUpdate;
	}

	// 現在のアニメーションを前回のアニメーションとする
	m_lastAnimHandle = m_currentAnimHandle;
	m_lastAnimCount = m_currentAnimCount;
	m_lastAnimSpeed = m_currentAnimSpeed;

	// 現在のアニメーションを設定
	m_currentAnimHandle = MV1AttachAnim(modelHandle, animIndex, -1);
	m_currentAnimCount = 0.0f;
	m_currentAnimSpeed = animSpeed;

	// アニメーションのブレンド終了までの時間を取得
	m_animBlendTime = blendTime;
	// アニメーションのブレンドカウントをリセットする
	m_animBlendCount = 0;

	// アニメーションの終了フラグをリセットする
	m_isAnimEnd = false;

	// ブレンドの時間が0以上の場合
	if (blendTime > 0)
	{
		// 更新処理をブレンド中のものにする
		m_blendUpdate = &Animation::BlendUpdate;
	}
	else
	{
		// 前回のアニメーションが存在する場合強制的に消去
		if (m_lastAnimHandle != -1)
		{
			// デタッチして消去
			MV1DetachAnim(modelHandle, m_lastAnimHandle);
			m_lastAnimHandle = -1;
		}
		// 通常の更新処理に変更
		m_blendUpdate = &Animation::NormalUpdate;
	}

	// ループする場合
	if (isLoop)
	{
		// 更新処理をループのものにする
		m_animUpdate = &Animation::LoopUpdate;
	}
	else
	{
		// 更新処理を一度だけのものにする
		m_animUpdate = &Animation::SingleUpdate;
	}
}

void Animation::NormalUpdate(int modelHandle)
{
	// 何もしない
}

void Animation::SingleUpdate(int modelHandle)
{
	//-----現在のアニメーションの更新-----//
	// 現在のアニメーションの再生時間を進める
	m_currentAnimCount += m_currentAnimSpeed;

	// 現在のアニメーションの総再生時間を取得
	float currentAnimTime = MV1GetAttachAnimTotalTime(modelHandle, m_currentAnimHandle);

	// 現在の時間がアニメーションの総再生時間を超えていたら
	if (m_currentAnimCount > currentAnimTime)
	{
		// アニメーションが終わったとする
		m_isAnimEnd = true;
		// アニメーションの終わりのフレームで止める
		m_currentAnimCount = currentAnimTime;
	}

	// アニメーションの再生時間を設定
	MV1SetAttachAnimTime(modelHandle, m_currentAnimHandle, m_currentAnimCount);

	//-----前回のアニメーションの更新-----//
	// 前回のアニメーションが存在する場合
	if (m_lastAnimHandle != -1)
	{
		// 前回のアニメーションの再生時間を進める
		m_lastAnimCount += m_lastAnimSpeed;

		// 前回のアニメーションの総再生時間を取得
		float lastAnimTime = MV1GetAttachAnimTotalTime(modelHandle, m_lastAnimHandle);

		// 現在の時間がアニメーションの総再生時間を超えていたら
		if (m_lastAnimCount > lastAnimTime)
		{
			// アニメーションの終わりのフレームで止める
			m_lastAnimCount = lastAnimTime;
		}

		// アニメーションの再生時間を設定
		MV1SetAttachAnimTime(modelHandle, m_lastAnimHandle, m_lastAnimCount);
	}
}

void Animation::LoopUpdate(int modelHandle)
{
	//-----現在のアニメーションの更新-----//
	// 現在のアニメーションの再生時間を進める
	m_currentAnimCount += m_currentAnimSpeed;

	// 現在のアニメーションの総再生時間を取得
	float currentAnimTime = MV1GetAttachAnimTotalTime(modelHandle, m_currentAnimHandle);

	// 現在の時間がアニメーションの総再生時間を超えていたら
	if (m_currentAnimCount > currentAnimTime)
	{
		// 総再生時間分引く
		m_currentAnimCount -= currentAnimTime;
	}

	// アニメーションの再生時間を設定
	MV1SetAttachAnimTime(modelHandle, m_currentAnimHandle, m_currentAnimCount);

	//-----前回のアニメーションの更新-----//
	// 前回のアニメーションが存在する場合
	if (m_lastAnimHandle != -1)
	{
		// 前回のアニメーションの再生時間を進める
		m_lastAnimCount += m_lastAnimSpeed;

		// 前回のアニメーションの総再生時間を取得
		float lastAnimTime = MV1GetAttachAnimTotalTime(modelHandle, m_lastAnimHandle);

		// 現在の時間がアニメーションの総再生時間を超えていたら
		if (m_lastAnimCount > lastAnimTime)
		{
			// 総再生時間分引く
			m_lastAnimCount -= lastAnimTime;
		}

		// アニメーションの再生時間を設定
		MV1SetAttachAnimTime(modelHandle, m_lastAnimHandle, m_lastAnimCount);
	}
}

void Animation::BlendUpdate(int modelHandle)
{
	// アニメーションのブレンド時間を計測
	m_animBlendCount++;

	// アニメーションが1つしか設定されていない場合
	if (m_lastAnimHandle == -1)
	{
		// ブレンドせずに通常通りにアニメーションが動くようにする
		MV1SetAttachAnimBlendRate(modelHandle, m_currentAnimHandle, 1.0f);
		// 通常の更新処理に変更
		m_blendUpdate = &Animation::NormalUpdate;
		// 念のためreturnする
		return;
	}
	else
	{
		// アニメーションのブレンド率
		float rate = 1.0f;

		// アニメーションのブレンド時間が0以上の場合は計算する
		if (m_animBlendTime > 0)
		{
			// アニメーションのブレンド率を計算
			rate = static_cast<float>(m_animBlendCount) / m_animBlendTime;
		}

		// アニメーションのブレンド率を設定
		// 現在のアニメーションは徐々にブレンド率が上がる
		MV1SetAttachAnimBlendRate(modelHandle, m_currentAnimHandle, rate);
		// 前回のアニメーションは徐々にブレンド率が下がる
		MV1SetAttachAnimBlendRate(modelHandle, m_lastAnimHandle, 1.0f - rate);

		// 割合が最大値を超えたら
		if (rate >= 1.0f)
		{
			// 前回のアニメーションが存在する場合
			if (m_lastAnimHandle != -1)
			{
				// デタッチして消去
				MV1DetachAnim(modelHandle, m_lastAnimHandle);
				m_lastAnimHandle = -1;
			}
			// 通常の更新処理に変更
			m_blendUpdate = &Animation::NormalUpdate;
			// 念のためreturnする
			return;
		}
	}

}
