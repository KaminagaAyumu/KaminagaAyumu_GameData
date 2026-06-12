#include <string>
#include "SoundManager.h"
#include "DxLib.h"
#include <algorithm>

namespace
{
	// マスターボリュームの最大音量の割合
	constexpr float kMaxMasterVolumeRate = 1.0f;

	// DxLibにおける最大音量の大きさ
	constexpr int kMaxVolume = 255;

	// デフォルトのSEとBGMの大きさ
	constexpr float kDefaultBGMBusVolume = 0.9f;
	constexpr float kDefaultSEBusVolume = 0.9f;

	// 1フレームの時間
	constexpr float kFrameTime = 1.0f / 60.0f;
}

SoundManager& SoundManager::GetInstance()
{
	static SoundManager instance;
	return instance;
}

void SoundManager::Init()
{
	// 全体の音量をデフォルト値に設定
	m_masterVolume = kMaxMasterVolumeRate;

	// 種別ごとに音量を設定
	m_busVolume = 
	{
		// デフォルトの音量を設定
		{SoundBus::BGM, kDefaultBGMBusVolume},
		{SoundBus::SE, kDefaultSEBusVolume}
	};

	// BGMの状態を初期化
	m_bgmPhase = BGMPhase::Idle;

	// BGMのスタックを初期化
	m_bgmStack.clear();
}

void SoundManager::End()
{
	// 登録されているサウンドのハンドルをすべてメモリから開放する
	for (auto& pair : m_soundClips)
	{
		// サウンドメモリの解放
		DeleteSoundMem(pair.second.handle);
	}
	// サウンドクリップのコンテナをクリア
	m_soundClips.clear();
}

void SoundManager::Update()
{
	if (m_bgmPhase != BGMPhase::CrossFading || !m_crossBGMInfo.isActive)
	{
		return;
	}

	auto& info = m_crossBGMInfo;


	// フェード値が0以上の場合
	if (info.durationSec <= info.elapsedSec)
	{
		if (info.fadeOutTrack)
		{
			info.fadeOutTrack->volume = info.outEnd;
			ChangeVolumeSoundMem(ToDxLibVolume(m_masterVolume *
				GetBusVolume(m_soundClips[info.fadeOutTrack->soundID].bus) *
				m_soundClips[info.fadeOutTrack->soundID].defaultRate *
				info.fadeOutTrack->volume),
				info.fadeOutTrack->handle);
			StopBGMTrack(*info.fadeOutTrack);
		}
		if (info.fadeInTrack)
		{
			info.fadeInTrack->volume = info.inEnd;
			ChangeVolumeSoundMem(ToDxLibVolume(m_masterVolume *
				GetBusVolume(m_soundClips[info.fadeInTrack->soundID].bus) *
				m_soundClips[info.fadeInTrack->soundID].defaultRate *
				info.fadeInTrack->volume),
				info.fadeInTrack->handle);
		}
		// クロスフェード情報を初期化
		info = {};
		m_bgmPhase = BGMPhase::Idle;
		return;
	}

	info.elapsedSec += kFrameTime;
	float t = std::clamp(info.elapsedSec / info.durationSec, 0.0f, 1.0f);

	float wOut = EqualPowerOut(t);
	float wIn = EqualPowerIn(t);

	// 音量を適用
	if (info.fadeOutTrack)
	{
		info.fadeOutTrack->volume = info.outStart * (1.0f - t) + info.outEnd * t;
		info.fadeOutTrack->volume *= wOut;
		
		const auto& clip = m_soundClips[info.fadeOutTrack->soundID];
		const int outVol255 = ToDxLibVolume(m_masterVolume * GetBusVolume(clip.bus) * clip.defaultRate * info.fadeOutTrack->volume);
		ChangeVolumeSoundMem(outVol255, info.fadeOutTrack->handle);
	}

	if (info.fadeInTrack)
	{
		info.fadeInTrack->volume = info.inStart * (1.0f - t) + info.inEnd * t;
		info.fadeInTrack->volume *= wIn;
		const auto& clip = m_soundClips[info.fadeInTrack->soundID];
		const int inVol255 = ToDxLibVolume(m_masterVolume * GetBusVolume(clip.bus) * clip.defaultRate * info.fadeInTrack->volume);
		ChangeVolumeSoundMem(inVol255, info.fadeInTrack->handle);
	}



	if (t >= 1.0f)
	{
		if (info.fadeOutTrack)
		{
			StopBGMTrack(*info.fadeOutTrack);
		}
		info = {};
		m_bgmPhase = BGMPhase::Idle;
	}

}

void SoundManager::SetMasterVolume(float volume)
{
	// 指定されたボリュームが0.0~1.0の範囲内になるようにする
	m_masterVolume = std::clamp(volume, 0.0f, kMaxMasterVolumeRate);
}

void SoundManager::SetBusVolume(SoundBus bus, float volume)
{
	// 指定されたボリュームが0.0~1.0の範囲内になるようにする
	m_busVolume[bus] = std::clamp(volume, 0.0f, kMaxMasterVolumeRate);
}

float SoundManager::GetBusVolume(SoundBus bus) const
{
	// サウンドの種類があるかどうかを判定
	auto it = m_busVolume.find(bus);
	// サウンドの種類がある場合
	if (it != m_busVolume.end())
	{
		return it->second; // サウンドの音量を返す
	}
	else
	{
		return kMaxMasterVolumeRate; // デフォルトの音量を返す(とりあえず1.0にしておく)
	}

}

bool SoundManager::LoadSoundClip(const std::string& soundID, const std::wstring& path, SoundBus bus, float volume, bool isLoop)
{
	// IDが既にロードされていたらこの時点でtrueを返す
	if (m_soundClips.count(soundID))
	{
		// IDに対応するサウンドクリップを取得
		auto& clip = m_soundClips[soundID];
		// サウンドのハンドルが有効なら
		if (clip.handle != -1)
		{
			// 参照カウントを増やす
			m_soundClips[soundID].refCount++;
			return true; // すでにロードできているのでtrueを返す
		}
	}
	// パスからサウンドデータをロード
	int handle = LoadSoundMem(path.c_str());
	// ハンドルの取得に失敗したらfalseを返す
	if (handle == -1) return false;
	// サウンドのコンテナに追加
	m_soundClips[soundID] = SoundClip{ handle, bus, volume, isLoop };
	// 参照カウントを増やす
	m_soundClips[soundID].refCount++;
	// ここまで来たらロードできているのでtrueを返す
	return true;
}

bool SoundManager::DeleteSoundClip(const std::string& soundID)
{
	// 指定されたIDのサウンドクリップを探す
	auto it = m_soundClips.find(soundID);
	// 指定されたIDのサウンドが見つからなかったらfalseを返す
	if (it == m_soundClips.end()) { return false; }

	// 参照カウントを減らす
	it->second.refCount--;

	// 参照カウントが0以下になったら
	if (it->second.refCount <= 0)
	{
		// サウンドのリソースを削除
		DeleteSoundMem(it->second.handle);
		// コンテナからも削除
		m_soundClips.erase(it);
	}

	// ここまで来たら削除できているのでtrueを返す
	return true;
}

void SoundManager::Play(const std::string& soundID, float volume, bool restart)
{
	auto it = m_soundClips.find(soundID);
	// 指定されたIDのサウンドが存在するかを判定
	if (it == m_soundClips.end()) return;
	// サウンドクリップを取得
	const SoundClip& clip = it->second;
	// ボリュームを適用
	ApplyVolumeToHandle(clip, volume);
	// ループ設定を適用
	int playType = clip.isLoop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK;
	// 再び鳴らすかどうかを判定してフラグを設定
	if (!restart)
	{
		// 既に再生中の場合は何もしない
		if (CheckSoundMem(clip.handle) == 1) return;
	}
	// サウンドを再生
	PlaySoundMem(clip.handle, playType);
}

void SoundManager::PlayBGM(const std::string& soundID, float fadeTime)
{
	// 現在のBGMを止めてから新しいBGMを再生する
	StopBGMTrack(m_bgmA);
	StopBGMTrack(m_bgmB);

	StartBGMOnTrack(m_bgmA, soundID, fadeTime > 0.0f ? 0.0f : 1.0f, nullptr); // フェードインがある場合は最初は音量0で再生

	if (fadeTime > 0.0f)
	{
		// BGMの状態を初期設定
		m_crossBGMInfo = {};
		m_crossBGMInfo.fadeOutTrack = nullptr;
		m_crossBGMInfo.fadeInTrack = &m_bgmA;
		m_crossBGMInfo.durationSec = fadeTime;
		m_crossBGMInfo.elapsedSec = 0.0f;
		m_crossBGMInfo.inStart = 0.0f;
		m_crossBGMInfo.inEnd = 1.0f;
		m_crossBGMInfo.isActive = true;
		m_bgmPhase = BGMPhase::CrossFading;
	}
	else
	{
		m_bgmPhase = BGMPhase::Idle;
	}

}

void SoundManager::CrossFadeBGM(const std::string& soundID, float fadeTime)
{
	if (!m_bgmA.isActive && !m_bgmB.isActive)
	{
		// どちらのトラックも再生されていない場合は通常のBGM再生を行う
		PlayBGM(soundID, 0.0f);
		return;
	}

	// フェードアウト中のトラックとフェードイン中のトラックを切り替える
	BGMTrack* currentTrack = nullptr;
	BGMTrack* newTrack = nullptr;

	if (m_bgmA.isActive && m_bgmB.isActive)
	{
		currentTrack = (m_bgmA.volume >= m_bgmB.volume) ? &m_bgmA : &m_bgmB;
		newTrack = (currentTrack == &m_bgmA) ? &m_bgmB : &m_bgmA;
	}
	else
	{
		currentTrack = (m_bgmA.isActive) ? &m_bgmA : &m_bgmB;
		newTrack = (currentTrack == &m_bgmA) ? &m_bgmB : &m_bgmA;
	}

	// 新しいトラックでBGMを開始
	StopBGMTrack(*newTrack); // 念のため停止しておく
	StartBGMOnTrack(*newTrack, soundID, 0.0f, currentTrack); // 最初は音量0で再生

	m_crossBGMInfo.fadeOutTrack = currentTrack;
	m_crossBGMInfo.fadeInTrack = newTrack;
	m_crossBGMInfo.durationSec = fadeTime;
	m_crossBGMInfo.elapsedSec = 0.0f;

	// クロスフェードの開始音量、終了音量を設定
	m_crossBGMInfo.outStart = currentTrack ? currentTrack->volume : 0.0f; // 現在のトラックが存在するかどうかで初期音量が決まる
	m_crossBGMInfo.outEnd = 0.0f; // 最初は0
	m_crossBGMInfo.inStart = 0.0f; // StartBGMOnTrackで0にしている
	m_crossBGMInfo.inEnd = 1.0f;

	// フェード時間があるならtrueとする
	m_crossBGMInfo.isActive = (fadeTime > 0.0f);

	// クロスフェードするかどうかでBGMのフェーズを変更
	m_bgmPhase = m_crossBGMInfo.isActive ? BGMPhase::CrossFading : BGMPhase::Idle;
}

void SoundManager::Stop(const std::string& soundID)
{
	auto it = m_soundClips.find(soundID);
	// 指定されたIDのサウンドが存在するかを判定
	if (it == m_soundClips.end()) return;
	// サウンドを止める
	StopSoundMem(it->second.handle);
}

void SoundManager::StopBGM(float fadeOutTime)
{
	if (fadeOutTime <= 0.0f)
	{
		StopBGMTrack(m_bgmA);
		StopBGMTrack(m_bgmB);
		m_bgmPhase = BGMPhase::Idle;
		return;
	}

	if (!m_bgmA.isActive && !m_bgmB.isActive)
	{
		// どちらのトラックも再生されていない場合は何もしない
		return;
	}

	// フェードアウト中のトラックとフェードイン中のトラックを切り替える
	BGMTrack* currentTrack = (m_bgmA.isActive) ? &m_bgmA : (m_bgmB.isActive) ? &m_bgmB : nullptr;

	if (!currentTrack) return;

	m_crossBGMInfo = {};
	m_crossBGMInfo.fadeOutTrack = currentTrack;
	m_crossBGMInfo.fadeInTrack = nullptr;

	m_crossBGMInfo.outStart = currentTrack->volume;
	m_crossBGMInfo.outEnd = 0.0f;
	m_crossBGMInfo.inStart = 0.0f;
	m_crossBGMInfo.inEnd = 0.0f;

	m_crossBGMInfo.durationSec = fadeOutTime;
	m_crossBGMInfo.elapsedSec = 0.0f;
	m_crossBGMInfo.isActive = true;

	m_bgmPhase = BGMPhase::CrossFading;

}

void SoundManager::BeginTemporaryBGM(const std::string& soundID, float fadeTime)
{
	// 現在のBGMを取得
	const BGMTrack* current = (m_bgmA.isActive ? &m_bgmA : (m_bgmB.isActive ? &m_bgmB : nullptr));

	if (current && !current->soundID.empty())
	{
		BGMState state;
		state.soundID = current->soundID;
		state.localVolume = current->volume;
		m_bgmStack.push_back(std::move(state));
	}
	else
	{
		// BGMがない場合はプッシュしない
	}

	// 一時的に流すBGMをクロスフェード
	CrossFadeBGM(soundID, fadeTime);
}

void SoundManager::EndTemporaryBGM(float fadeTime)
{
	// 記憶しておいたBGMがない場合
	if (m_bgmStack.empty())
	{
		// 処理をしない
		return;
	}

	// 最後に積まれたスタックのBGM情報を取得
	BGMState state = std::move(m_bgmStack.back());
	// スタックから消去
	m_bgmStack.pop_back();

	CrossFadeBGM(state.soundID, fadeTime);
}

void SoundManager::ClearBGMStack()
{
	m_bgmStack.clear();
}

void SoundManager::ApplyVolumeToHandle(const SoundClip& clip, float volume) const
{
	// サウンドの音量を取得
	float busVolume = GetBusVolume(clip.bus);
	// 音量の割合を計算
	float rate = m_masterVolume * busVolume * clip.defaultRate * volume;
	// 最終的なボリュームを取得
	int vol = ToDxLibVolume(rate);
	// ハンドルに音量を設定
	ChangeVolumeSoundMem(vol, clip.handle);
}

int SoundManager::ToDxLibVolume(float rate) const
{
	// 割合を0~1の間に収める
	const float clamped = std::clamp(rate, 0.0f, 1.0f);

	const int vol = static_cast<int>(std::lround(clamped * kMaxVolume));

	// 0~255の間に収まるようにする
	return std::clamp(vol, 0, kMaxVolume);
}

void SoundManager::StartBGMOnTrack(BGMTrack& track, const std::string& soundID, float volume, const BGMTrack* other)
{

	auto it = m_soundClips.find(soundID);
	if (it == m_soundClips.end() || it->second.handle == -1)
	{
		track = BGMTrack{};
		return;
	}

	const auto& clip = it->second;
	int handleToUse = clip.handle;
	bool needDuplicate = false;

	if (other && other->isActive && other->soundID == soundID)
	{
		needDuplicate = true;
	}

	if (!needDuplicate && CheckSoundMem(handleToUse) == 1)
	{
		needDuplicate = true;
	}

	if (needDuplicate)
	{
		int dup = DuplicateSoundMem(clip.handle);
		// ハンドルの取得に失敗したときはトラックを初期化
		if (dup == -1)
		{
			track = BGMTrack{};
			return;
		}
		handleToUse = dup;
		track.isOwnsHandle = true;
	}
	else
	{
		track.isOwnsHandle = false;
	}

	track.soundID = soundID;
	track.handle = handleToUse;
	track.volume = volume;
	track.isActive = true;


	ChangeVolumeSoundMem(ToDxLibVolume(m_masterVolume * GetBusVolume(clip.bus) * clip.defaultRate * track.volume), track.handle);
	int r = PlaySoundMem(track.handle, DX_PLAYTYPE_LOOP); // ループ前提で再生

	if (other)
	{
		//printfDx(L"other.handle = %d Check = %d sameID = %d\n", other->handle, CheckSoundMem(other->handle), (int)(other->soundID == soundID));
	}

}

void SoundManager::StopBGMTrack(BGMTrack& track)
{
	// トラックがアクティブかどうかを判定
	if (track.isActive && track.handle != -1)
	{
		StopSoundMem(track.handle);
		if (track.isOwnsHandle)
		{
			DeleteSoundMem(track.handle);
		}
	}
	track = BGMTrack{}; // トラック情報をリセット
}

float SoundManager::EqualPowerOut(float t)
{
	return std::cos(0.5f * DX_PI_F * t);
}

float SoundManager::EqualPowerIn(float t)
{
	return std::sin(0.5f * DX_PI_F * t);
}