#include "Animator.h"
#include "Draw/Drawable3D.h"
#include "../GameObject.h"
#include "../../Main/Application.h"
#include "DxLib.h"
#include <cassert>

namespace
{
	// アニメーションの最大ブレンド率
	constexpr float kMaxBlendRate = 1.0f;
}

MyLib::Animator::Animator() : 
	m_modelHandle(-1),
	m_currentAnimHandle(-1),
	m_currentAnimIndex(-1),
	m_lastAnimHandle(-1),
	m_animBlendFrame(0.0f),
	m_animBlendCount(0.0f),
	m_currentAnimCount(0.0f),
	m_lastAnimCount(0.0f),
	m_currentAnimSpeed(0.0f),
	m_lastAnimSpeed(0.0f),
	m_isAnimEnd(false),
	m_blendUpdate(&Animator::NormalUpdate),
	m_animUpdate(&Animator::SingleUpdate)
{

}

void MyLib::Animator::Init(std::weak_ptr<MyLib::GameObject> parent)
{
	m_pParent = parent;
}

void MyLib::Animator::Start()
{
	std::shared_ptr<MyLib::GameObject> pParent = m_pParent.lock();

	std::shared_ptr<MyLib::Drawable3D> pDrawable = pParent->GetComponent<MyLib::Drawable3D>().lock();

	if (!pDrawable)
	{
		assert(false && "Animator : Drawable3Dコンポーネントがありません");
	}

	// アニメーションを行うモデルのハンドルを取得する
	m_modelHandle = pDrawable->GetModelHandle(Model::ModelSlot::Main);

	// 更新処理を初期状態のものにする(ループしないアニメーションを行う)
	m_blendUpdate = &Animator::NormalUpdate;
	m_animUpdate = &Animator::SingleUpdate;
}

void MyLib::Animator::Update()
{
	if (m_modelHandle != -1)
	{
		// 現在の更新処理を行う
		(this->*m_blendUpdate)(m_modelHandle);
		(this->*m_animUpdate)(m_modelHandle);
	}
}

void MyLib::Animator::End()
{
}

void MyLib::Animator::SetAnimation(int modelHandle, int animIndex)
{
	// アニメーションのハンドルを取得
	m_currentAnimHandle = MV1AttachAnim(modelHandle, animIndex, -1);
}

void MyLib::Animator::ChangeAnimation(int animIndex, float animSpeed, float blendFrame, bool isLoop)
{
	// アニメーション番号が無効なものの場合処理をしない
	if (animIndex == -1)
	{
		return;
	}

	// 同じアニメーションで、ブレンドが終了しているものはスピードのみ更新する
	if (animIndex == m_currentAnimIndex && m_animUpdate == &Animator::NormalUpdate)
	{
		m_currentAnimSpeed = animSpeed;
		return;
	}

	m_currentAnimIndex = animIndex;

	// すでにアニメーションブレンド中の場合
	if (m_blendUpdate == &Animator::BlendUpdate)
	{
		// 前回のアニメーションが存在する場合強制的に消去
		if (m_lastAnimHandle != -1)
		{
			// デタッチして消去
			MV1DetachAnim(m_modelHandle, m_lastAnimHandle);
			m_lastAnimHandle = -1;
		}
		// 通常の更新処理に変更
		m_blendUpdate = &Animator::NormalUpdate;
	}

	// 現在のアニメーションを前回のアニメーションとする
	m_lastAnimHandle = m_currentAnimHandle;
	m_lastAnimCount = m_currentAnimCount;
	m_lastAnimSpeed = m_currentAnimSpeed;

	// 現在のアニメーションを設定
	m_currentAnimHandle = MV1AttachAnim(m_modelHandle, animIndex, -1);
	m_currentAnimCount = 0.0f;
	m_currentAnimSpeed = animSpeed;

	// アニメーションのブレンド終了までの時間を取得
	m_animBlendFrame = blendFrame;
	// アニメーションのブレンドカウントをリセットする
	m_animBlendCount = 0;

	// アニメーションの終了フラグをリセットする
	m_isAnimEnd = false;

	// ブレンドの時間が0以上の場合
	if (blendFrame > 0.0f)
	{
		// 更新処理をブレンド中のものにする
		m_blendUpdate = &Animator::BlendUpdate;
	}
	else
	{
		// 前回のアニメーションが存在する場合強制的に消去
		if (m_lastAnimHandle != -1)
		{
			// デタッチして消去
			MV1DetachAnim(m_modelHandle, m_lastAnimHandle);
			m_lastAnimHandle = -1;
		}
		// 通常の更新処理に変更
		m_blendUpdate = &Animator::NormalUpdate;
	}

	// ループする場合
	if (isLoop)
	{
		// 更新処理をループのものにする
		m_animUpdate = &Animator::LoopUpdate;
	}
	else
	{
		// 更新処理を一度だけのものにする
		m_animUpdate = &Animator::SingleUpdate;
	}
}

void MyLib::Animator::ChangeAnimation(const std::wstring_view& animName, float animSpeed, float blendFrame, bool isLoop)
{
	// アニメーション名からアニメーション番号を取得する
	int animIndex = MV1GetAnimIndex(m_modelHandle, animName.data());
	// アニメーションを変更する
	ChangeAnimation(animIndex, animSpeed, blendFrame, isLoop);
}

void MyLib::Animator::ChangeAnimation(const std::wstring& animName, float animSpeed, float blendFrame, bool isLoop)
{
	// アニメーション名からアニメーション番号を取得する
	int animIndex = MV1GetAnimIndex(m_modelHandle, animName.c_str());
	// アニメーションを変更する
	ChangeAnimation(animIndex, animSpeed, blendFrame, isLoop);
}

void MyLib::Animator::SingleUpdate(int modelHandle)
{
	// タイムスケールを取得する
	float timeScale = Application::GetInstance().GetTimeScale();

	//-----現在のアニメーションの更新-----//
	// 現在のアニメーションの再生時間を進める
	m_currentAnimCount += m_currentAnimSpeed * timeScale;

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
		m_lastAnimCount += m_lastAnimSpeed * timeScale;

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

void MyLib::Animator::LoopUpdate(int modelHandle)
{
	// タイムスケールを取得する
	float timeScale = Application::GetInstance().GetTimeScale();

	//-----現在のアニメーションの更新-----//
	// 現在のアニメーションの再生時間を進める
	m_currentAnimCount += m_currentAnimSpeed * timeScale;

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
		m_lastAnimCount += m_lastAnimSpeed * timeScale;

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

void MyLib::Animator::NormalUpdate(int modelHandle)
{
	// 何もしない
}

void MyLib::Animator::BlendUpdate(int modelHandle)
{
	// タイムスケールを取得する
	float timeScale = Application::GetInstance().GetTimeScale();

	// アニメーションのブレンド時間を計測
	m_animBlendCount += timeScale;

	// アニメーションが1つしか設定されていない場合
	if (m_lastAnimHandle == -1)
	{
		// ブレンドせずに通常通りにアニメーションが動くようにする
		MV1SetAttachAnimBlendRate(modelHandle, m_currentAnimHandle, 1.0f);
		// 通常の更新処理に変更
		m_blendUpdate = &Animator::NormalUpdate;
		// 念のためreturnする
		return;
	}
	else
	{
		// アニメーションのブレンド率
		float rate = kMaxBlendRate;

		// アニメーションのブレンド時間が0以上の場合は計算する
		if (m_animBlendFrame > 0)
		{
			// アニメーションのブレンド率を計算
			rate = m_animBlendCount / m_animBlendFrame;
		}

		// アニメーションのブレンド率を設定
		// 現在のアニメーションは徐々にブレンド率が上がる
		MV1SetAttachAnimBlendRate(modelHandle, m_currentAnimHandle, rate);
		// 前回のアニメーションは徐々にブレンド率が下がる
		MV1SetAttachAnimBlendRate(modelHandle, m_lastAnimHandle, kMaxBlendRate - rate);

		// 割合が最大値を超えたら
		if (rate >= kMaxBlendRate)
		{
			// 前回のアニメーションが存在する場合
			if (m_lastAnimHandle != -1)
			{
				// デタッチして消去
				MV1DetachAnim(modelHandle, m_lastAnimHandle);
				m_lastAnimHandle = -1;
			}
			// 通常の更新処理に変更
			m_blendUpdate = &Animator::NormalUpdate;
			// 念のためreturnする
			return;
		}
	}
}
