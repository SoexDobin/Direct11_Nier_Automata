#include "SoundManager.h"

NS_BEGIN(Engine)

SoundManager::SoundManager() : EngineManager{}
	, m_System{nullptr}
{
	for (int i = 0; i < ETOI(SOUNDCHANNEL::MAX_CHANNELS); ++i)
	{
		m_SoundChannel[i] = nullptr;
	}
}

SoundManager::~SoundManager()
{
	Clear_SoundSources();
}

HRESULT SoundManager::Initialize_Prototype()
{
	EngineManager::Initialize_Prototype();

	FMOD_System_Create(&m_System, FMOD_VERSION);
	FMOD_System_Init(m_System, ETOI(SOUNDCHANNEL::MAX_CHANNELS), FMOD_INIT_NORMAL, nullptr);

	return S_OK;
}

HRESULT SoundManager::Initialize(void *arg)
{
	return S_OK;
}

void SoundManager::PlaySoundFxOnce(const wstring& soundKey, SOUNDCHANNEL id, Float volume)
{
	auto iter = m_SoundSource.find(soundKey);
	if (iter == m_SoundSource.end())
	{
		return;
	}

	int32 iID = ETOI(id);

	FMOD_BOOL bPlay = FALSE;
	if (m_SoundChannel[iID])
	{
		FMOD_Channel_IsPlaying(m_SoundChannel[iID], &bPlay);
	}

	if (!bPlay)
	{
		FMOD_System_PlaySound(m_System, iter->second.pSound, nullptr, FALSE, &m_SoundChannel[iID]);
	}

	if (volume >= 0.f)
	{
		iter->second.volume = volume;
	}

	FMOD_Channel_SetVolume(m_SoundChannel[iID], iter->second.volume);
	FMOD_System_Update(m_System);
}

void SoundManager::PlaySoundFx(const wstring &soundKey, SOUNDCHANNEL id, Float volume)
{
	auto iter = m_SoundSource.find(soundKey);
	if (iter == m_SoundSource.end())
	{
		return;
	}

	int32 iID = ETOI(id);

	FMOD_System_PlaySound(m_System, iter->second.pSound, nullptr, FALSE, &m_SoundChannel[iID]);

	if (volume >= 0.f)
	{
		iter->second.volume = volume;
	}

	FMOD_Channel_SetVolume(m_SoundChannel[iID], iter->second.volume);
	FMOD_System_Update(m_System);
}

HRESULT SoundManager::StopChannel(SOUNDCHANNEL id)
{
	int iID = ETOI(id);
	if (m_SoundChannel[iID])
	{
		FMOD_Channel_Stop(m_SoundChannel[iID]);
	}

	return S_OK;
}

HRESULT SoundManager::StopAll()
{
	for (int i = 0; i < ETOI(SOUNDCHANNEL::MAX_CHANNELS); ++i)
	{
		if (m_SoundChannel[i])
		{
			FMOD_Channel_Stop(m_SoundChannel[i]);
		}
	}

	return S_OK;
}

HRESULT SoundManager::SetChannelVolume(SOUNDCHANNEL id, Float volume)
{
	int iID = ETOI(id);
	
	if (m_SoundChannel[iID])
	{
		FMOD_Channel_SetVolume(m_SoundChannel[iID], volume);
		FMOD_System_Update(m_System);
	}

	return S_OK;
}

HRESULT SoundManager::LoadSoundFile(const wstring& path)
{
	std::wstring basePath = path;
	if (!basePath.empty())
	{
		Char back = basePath.back();
		if (back != L'/' && back != L'\\')
		{
			basePath += L'/';
		}
	}

	vector<wstring> extensions = { L"*.wav", L"*.mp3" };
	bool bFound = false;

	for (const auto& ext : extensions)
	{
		std::wstring wsSearchPath = basePath + ext;

		_wfinddata_t fd;
		intptr_t hFind = _wfindfirst(wsSearchPath.c_str(), &fd);
		if (hFind == -1)
		{
			continue;
		}

		bFound = true;

		do
		{
			std::wstring wsFullPath = basePath + fd.name;

			char szFullPath[MAX_PATH] = {};
			WideCharToMultiByte(CP_ACP, 0,
								wsFullPath.c_str(), -1,
								szFullPath, MAX_PATH,
								nullptr, nullptr);

			FMOD_SOUND *pSound = nullptr;
			FMOD_RESULT eRes = FMOD_System_CreateSound(m_System,
													   szFullPath,
													   FMOD_DEFAULT,
													   0,
													   &pSound);

			if (eRes == FMOD_OK)
			{
				std::wstring wsKey = fd.name;

				tagSoundInfo info;
				info.pSound = pSound;
				info.volume = 1.0f;

				m_SoundSource.emplace(wsKey, info);
			}

		} while (_wfindnext(hFind, &fd) != -1);

		_findclose(hFind);
	}

	if (!bFound)
	{
		// MSG_BOX("There is no Sound File");
		return E_FAIL;
	}

	FMOD_System_Update(m_System);

	return S_OK;
}

HRESULT SoundManager::Load_Sound(const wstring& soundTag, const wstring& soundFilePath)
{
	char szFullPath[MAX_PATH] = {};
	WideCharToMultiByte(CP_ACP, 0,
						soundFilePath.c_str(), -1,
						szFullPath, MAX_PATH,
						nullptr, nullptr);

	FMOD_SOUND *pSound = nullptr;
	FMOD_RESULT eRes = FMOD_System_CreateSound(m_System,
											   szFullPath,
											   FMOD_DEFAULT,
											   0,
											   &pSound);

	if (eRes == FMOD_OK)
	{
		tagSoundInfo info;
		info.pSound = pSound;
		info.volume = 1.0f;

		if (m_SoundSource.find(soundTag) != m_SoundSource.end())
		{
			FMOD_Sound_Release(m_SoundSource[soundTag].pSound);
		}

		m_SoundSource[soundTag] = info;
		FMOD_System_Update(m_System);
		return S_OK;
	}

	return E_FAIL;
}

void SoundManager::PlaySoundLoopSection(
	const wstring& soundKey,
	SOUNDCHANNEL id,
	Float volume,
	uint32 loopStartMs,
	uint32 loopEndMs,
	Bool playIntro)
{
	auto iter = m_SoundSource.find(soundKey);
	if (iter == m_SoundSource.end())
	{
		return;
	}

	int iID = ETOI(id);
	FMOD_SOUND *pSound = iter->second.pSound;

	unsigned int lenMs = 0;
	FMOD_Sound_GetLength(pSound, &lenMs, FMOD_TIMEUNIT_MS);

	if (lenMs == 0)
	{
		return;
	}
	if (loopStartMs > lenMs) {
		loopStartMs = lenMs;
	}
	if (loopEndMs > lenMs) {
		loopEndMs = lenMs;
	}
	if (loopEndMs <= loopStartMs + 1)
	{
		return;
	}

	if (m_SoundChannel[iID])
	{
		FMOD_Channel_Stop(m_SoundChannel[iID]);
	}

	FMOD_System_PlaySound(m_System, pSound, nullptr, TRUE, &m_SoundChannel[iID]);

	FMOD_CHANNEL *ch = m_SoundChannel[iID];
	if (!ch)
	{
		return;
	}

	FMOD_Channel_SetMode(ch, FMOD_LOOP_NORMAL);
	FMOD_Channel_SetLoopPoints(ch,
							   loopStartMs, FMOD_TIMEUNIT_MS,
							   loopEndMs, FMOD_TIMEUNIT_MS);

	if (!playIntro)
	{
		FMOD_Channel_SetPosition(ch, loopStartMs, FMOD_TIMEUNIT_MS);
	}

	if (volume >= 0.f)
	{
		iter->second.volume = volume;
	}

	FMOD_Channel_SetVolume(ch, iter->second.volume);
	FMOD_Channel_SetPaused(ch, FALSE);

	FMOD_System_Update(m_System);
}

HRESULT SoundManager::Clear_SoundSources()
{
	for (auto& Mypair : m_SoundSource)
	{
		if (Mypair.second.pSound)
		{
			FMOD_Sound_Release(Mypair.second.pSound);
		}
	}
	m_SoundSource.clear();

	if (m_System)
	{
		FMOD_System_Release(m_System);
		FMOD_System_Close(m_System);
	}

	return S_OK;
}



Unique<SoundManager> SoundManager::Create()
{
	auto instance = make_unique<SoundManager>();

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Clone : SoundManager");
		return nullptr;
	}

	return instance;
}

NS_END
