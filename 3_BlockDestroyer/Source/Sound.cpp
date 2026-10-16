#include "Sound.h"
#include "DxLib.h"
Sound::Sound() :
	m_soundHandle(-1)
{
}

void Sound::Init(int soundHandle)
{
	m_soundHandle = soundHandle;
}

void Sound::Play()
{
	PlaySoundMem(m_soundHandle, DX_PLAYTYPE_BACK, true);
}

void Sound::Play(int soundHandle)
{
	PlaySoundMem(soundHandle, DX_PLAYTYPE_BACK, true);
}

void Sound::PlayLoop()
{
	PlaySoundMem(m_soundHandle, DX_PLAYTYPE_LOOP, true);
}

void Sound::PlayLoop(int soundHandle)
{
	PlaySoundMem(soundHandle, DX_PLAYTYPE_LOOP, true);
}

void Sound::Stop()
{
	StopSoundMem(m_soundHandle);
}

void Sound::Stop(int soundHandle)
{
	StopSoundMem(soundHandle);
}

void Sound::ChangeVolume(int volume)
{
	ChangeVolumeSoundMem(volume, m_soundHandle);
}

void Sound::ChangeVolume(int volume, int soundHandle)
{
	ChangeVolumeSoundMem(volume, soundHandle);
}

