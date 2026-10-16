#pragma once
#include "Vector2.h"
#include "DxLib.h" 
class Animation
{
public:
	Animation();
	virtual ~Animation() = default;
	
	void Init(int graphHandle, int maxAnimCount, Vector2 pos, Vector2 animSize);
	void Update();
	void Draw();


	void SetPos(Vector2 pos) { m_pos = pos; }

	void PlayOneShot();

private:
	int m_graphHandle;

	int m_frameCount;

	int m_maxAnimCount;

	int m_animCount;

	bool m_isPlaying;

	Vector2 m_pos;

	Vector2 m_animSize;

};

