#pragma once

#include "EngineManager.h"

NS_BEGIN(Engine)

class SoundManager : public EngineManager
{
public:
	typedef struct tagSoundInfo
	{
		FMOD_SOUND *pSound = nullptr;
		Float volume = 1.0f;
	} SOUND_DESC;

public:
	explicit SoundManager();
	virtual ~SoundManager() override;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void *arg) override;

public:
	void PlaySoundFxOnce(const wstring& soundKey, SOUNDCHANNEL id, Float volume = -1.f);
	void PlaySoundFx(const wstring& soundKey, SOUNDCHANNEL id, Float volume = -1.f);
	HRESULT StopChannel(SOUNDCHANNEL id);
	HRESULT StopAll();
	HRESULT SetChannelVolume(SOUNDCHANNEL id, Float volume);
	HRESULT LoadSoundFile(const wstring& path);
	HRESULT Load_Sound(const wstring& soundTag, const wstring& soundFilePath);
	void PlaySoundLoopSection(const wstring& soundKey,
							  SOUNDCHANNEL id,
							  Float volume,
							  uint32 loopStartMs,
							  uint32 loopEndMs,
							  Bool playIntro);

public:
	HRESULT Clear_SoundSources();

private:
	unordered_map<wstring, SOUND_DESC> m_SoundSource;
	FMOD_CHANNEL *m_SoundChannel[ETOI(SOUNDCHANNEL::MAX_CHANNELS)] = {};
	FMOD_SYSTEM *m_System = nullptr;

public:
	static Unique<SoundManager> Create();
};

NS_END
