#pragma once
class Sound
{
public:
	Sound();
	virtual ~Sound() = default;

	void Init(int soundHandle);
	void Play();
	void Play(int soundHandle);
	void PlayLoop();
	void PlayLoop(int soundHandle);
	void Stop();
	void Stop(int soundHandle);
	void ChangeVolume(int volume);
	void ChangeVolume(int volume, int soundHandle);

private:
	int m_soundHandle;

};
