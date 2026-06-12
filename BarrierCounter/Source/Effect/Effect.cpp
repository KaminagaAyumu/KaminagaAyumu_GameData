#include "Effect.h"
#include "EffekseerForDXLib.h"

Effect::Effect() : 
	m_effectHandle(-1)
{

}

Effect::~Effect()
{

}

void Effect::Init()
{
	// エフェクトハンドルの取得をしていなければ警告を表示
	if (m_effectHandle == -1)
	{
#ifdef _DEBUG
		printfDx(L"エフェクトのハンドルがエフェクトクラスに入っていない\n");
#endif // _DEBUG
	}
}

void Effect::End()
{
	// エフェクトを止める
	StopEffekseer3DEffect(m_effectHandle);
}

void Effect::Update()
{
}

void Effect::SetHandle(int handle)
{
	// ハンドルのエフェクトを再生する
	m_effectHandle = PlayEffekseer3DEffect(handle);
}

void Effect::StopEffect()
{
	// エフェクトを止める
	StopEffekseer3DEffect(m_effectHandle);
}

void Effect::SetPos(const Vector3& pos)
{
	m_pos = pos;

	SetPosPlayingEffekseer3DEffect(m_effectHandle, m_pos.ToDxLibVector().x, m_pos.ToDxLibVector().y, m_pos.ToDxLibVector().z);
}

bool Effect::IsAlive() const
{
	// 再生されている時はこの関数の引数が0になっている
	return IsEffekseer3DEffectPlaying(m_effectHandle) == 0;
}
