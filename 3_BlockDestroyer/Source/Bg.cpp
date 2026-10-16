#include "Bg.h"
#include "DxLib.h"
#include "Game.h"

Bg::Bg() :
	m_bgHandle(-1)
{

}

Bg::~Bg()
{

}

void Bg::Init(int handle)
{
	m_bgHandle = handle;
}

void Bg::End()
{

}

void Bg::Update()
{

}

void Bg::Draw()
{
	DrawRotaGraph(Game::kScreenWidth / 2, Game::kScreenHeight / 2, 1.0f, 0, m_bgHandle, false);
}

