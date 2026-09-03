#include "pch.h"
#include "LevelTitle.h"
#include <SpdLogger.h>
#include <Game.h>

#include "ClientSettingManager.h"
#include "LevelGamePlay.h"
#include "LevelLoading.h"
#include "Loader.h"
#include "LoadingFadeIn.h"
#include "LoadingFadeOut.h"

LevelTitle::LevelTitle(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Level { device, context } {}

HRESULT LevelTitle::Initialize(void* arg)
{
	if (FAILED(ClientSettingManager::GetInstance()->Load_LevelData(LEVEL::TITLE)))
		return E_FAIL;;

	Ready_TitleUI();

	return Level::Initialize(arg);
}

void LevelTitle::On_Destroy()
{
	Level::On_Destroy();
}

void LevelTitle::Update_Level(Float timeDelta)
{
	UNREFERENCED_PARAMETER(timeDelta);

	if (!m_IsIntroPlayed)
	{
		GAME_INSTANCE->PlaySoundFXOnce(L"Title_Intro", SOUNDCHANNEL::CHANNEL_1);
		m_IsIntroPlayed = true;
	}


	if (m_FadeOut->Is_FadeFinished())
	{
		if (!m_FadeIn->Is_Active() && GAME_INSTANCE->Get_DIKeyState(DIK_SPACE) & 0x80)
		{
			m_FadeIn->Set_Active(true);
			GAME_INSTANCE->PlaySoundFXOnce(L"Title_Tap", SOUNDCHANNEL::CHANNEL_2);
		}
			

		if (m_FadeIn->Is_FadeFinished())
		{
			GAME_INSTANCE->Change_Level(ETOI(LEVEL::LOADING), LevelLoading::Create(m_Device, m_Context, LEVEL::GAMEPLAY, true));
		}
	}
}

HRESULT LevelTitle::Render_Level()
{
	return S_OK;
}

void LevelTitle::Ready_TitleUI()
{
	LoadingFadeIn::FADE_IN_DESC fadeIn{};
	fadeIn.fadeSpeed = 0.35f;
	m_FadeIn = GAME_INSTANCE->Instantiate<LoadingFadeIn>(L"LoadingFadeIn", ETOI(LEVEL::TITLE), &fadeIn);


	LoadingFadeOut::FADE_OUT_DESC fadeOut{};
	fadeOut.fadeSpeed = 0.35f;
	m_FadeOut = GAME_INSTANCE->Instantiate<LoadingFadeOut>(L"LoadingFadeOut", ETOI(LEVEL::TITLE), &fadeOut);
};

Shared<LevelTitle> LevelTitle::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto titleLevel = make_shared<LevelTitle>(device, context);

	if (FAILED(titleLevel->Initialize(nullptr))) {
		LOG_ERROR(L"Failed To Create LoadingBackground");
		return nullptr;
	}

	return titleLevel;
}
