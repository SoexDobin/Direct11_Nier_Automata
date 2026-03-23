#include "pch.h"
#include "LevelTitle.h"
#include <SpdLogger.h>
#include <Game.h>

#include "ClientSettingManager.h"
#include "LevelGamePlay.h"
#include "LevelLoading.h"
#include "Loader.h"
#include "LoadingFade.h"

LevelTitle::LevelTitle(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Level { device, context } {}

HRESULT LevelTitle::Initialize(void* arg)
{
	if (FAILED(ClientSettingManager::GetInstance()->Load_LevelData(LEVEL::TITLE)))
		return E_FAIL;

	Ready_TitleUI();

	return Level::Initialize(arg);
}

void LevelTitle::On_Destroy()
{
	Level::On_Destroy();
}

void LevelTitle::Update_Level(Float timeDelta)
{
	if (!m_IsIntroPlayed)
	{
		GAME_INSTANCE->PlaySoundFXOnce(L"Title_Intro", SOUNDCHANNEL::CHANNEL_1);
		m_IsIntroPlayed = true;
	}


	m_FadeOut->Update(timeDelta);

	if (m_FadeOut->Fade_End())
	{
		if (!m_FadeIn->Is_Active() && GAME_INSTANCE->Get_DIKeyState(DIK_SPACE) & 0x80)
		{
			m_FadeIn->Set_Active(true);
			GAME_INSTANCE->PlaySoundFXOnce(L"Title_Tap", SOUNDCHANNEL::CHANNEL_2);
		}
			

		m_FadeIn->Update(timeDelta);
		if (m_FadeIn->Fade_End())
		{
			GAME_INSTANCE->Change_Level(ETOI(LEVEL::LOADING), LevelLoading::Create(m_Device, m_Context, LEVEL::GAMEPLAY, true));
		}
	}
}

HRESULT LevelTitle::Render_Level()
{
	m_FadeOut->Render();
	m_FadeIn->Render();

	return S_OK;
}

void LevelTitle::Ready_TitleUI()
{
	LoadingFade::LOADING_FADE_UI_DESC fadeIn{};
	fadeIn.fadeSpeed = 0.35f;
	fadeIn.isFadeIn = true;
	fadeIn.isHuman = true;
	m_FadeIn = GAME_INSTANCE->Instantiate<LoadingFade>(L"LoadingFade", ETOI(LEVEL::TITLE), &fadeIn);
	LoadingFade::LOADING_FADE_UI_DESC fadeOut{};
	fadeOut.fadeSpeed = 0.35f;
	fadeOut.isFadeOut = true;
	fadeOut.isHuman = false;
	m_FadeOut = GAME_INSTANCE->Instantiate<LoadingFade>(L"LoadingFade", ETOI(LEVEL::TITLE), &fadeOut);
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
