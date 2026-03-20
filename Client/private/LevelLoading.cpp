#include "pch.h"
#include "LevelLoading.h"

#include "Game.h"
#include "Loader.h"
#include <SpdLogger.h>
#include "LoadingFade.h"

#include "ClientSettingManager.h"
#include "LevelGamePlay.h"
#include "LevelTitle.h"
#include "LoadingBackground.h"
#include "LoadingLogo.h"
#include "LoadingPixelPanel.h"
#include "StaticCamera.h"

LevelLoading::LevelLoading(const ComPtr<ID3D11Device> &device,
                           const ComPtr<ID3D11DeviceContext> &context)
    : Level{device, context}, m_Loader{nullptr}, m_NextLevel{LEVEL::LEVEL_END} {
}

HRESULT LevelLoading::Initialize(void *arg) {
    m_NextLevel = *static_cast<LEVEL*>(arg);

    if (SUCCEEDED(ClientSettingManager::GetInstance()->Sync_TextureJson_FromCSV())) {
        if (FAILED(ClientSettingManager::GetInstance()->Load_Textures_FromJson(LEVEL::LOADING)))
        {
            LOG_ERROR(L"Failed to load Loading Texture");
            return E_FAIL;
        }
    }
    if (SUCCEEDED(ClientSettingManager::GetInstance()->Sync_TextureJson_FromCSV())) {
        if (FAILED(ClientSettingManager::GetInstance()->Load_Textures_FromJson(LEVEL::STATIC))) {
            LOG_ERROR(L"Failed to Load Textures");
            return E_FAIL;
        }
    }

    if (FAILED(ClientSettingManager::GetInstance()->Ready_Client_Prototypes(LEVEL::LOADING)))
    {
        LOG_ERROR(L"Failed to load Loading Prototypes");
        return E_FAIL;
    }

    Ready_LoadingUI();

    m_Loader = Loader::Create(m_Device, m_Context, m_NextLevel, shared_from_this(), true);
    if (nullptr == m_Loader)
        return E_FAIL;

    m_IsFinished = m_Loader->Is_Finished();

    return Level::Initialize(arg);
}

void LevelLoading::On_Destroy() { Level::On_Destroy(); }

void LevelLoading::Update_Level(Float timeDelta) 
{
    m_StaticCamera->Update_CameraTransform(timeDelta);
    m_Background->Update(timeDelta);
    m_PixelPanel->Update(timeDelta);
    m_Logo->Update(timeDelta);

    if (!m_FadeOut->Fade_End())
    {
        m_FadeOut->Update(timeDelta);
    }

    if (true == m_IsFinished && m_FadeOut->Fade_End())
    {
        Transition_To_NextLevel(timeDelta);
    }
}

void LevelLoading::Update_LoadLevel(Float timeDelta)
{
    m_Background->Update(timeDelta);
    m_PixelPanel->Update(timeDelta);
    m_Logo->Update(timeDelta);
    
    if (!m_FadeOut->Fade_End())
    {
        m_FadeOut->Update(timeDelta);
    }
    if (true == m_IsFinished && m_FadeOut->Fade_End())
    {
        Transition_To_NextLevel(timeDelta);
    }
}

void LevelLoading::Transition_To_NextLevel(Float timeDelta)
{
	if (!m_FadeIn->Fade_End())
	{
		m_FadeIn->Set_Active(true);
		m_FadeIn->Update(timeDelta); // Approximation or passed timeDelta
	}
	else
	{
		if (ClientSettingManager::GetInstance()->AutoTransitionLevel(LEVEL::LOADING, m_NextLevel))
		{
			switch (ETOI(m_NextLevel))
			{
			case ETOI(LEVEL::TITLE):
				GAME_INSTANCE->Clear_Resource(ETOI(LEVEL::LOADING));
				GAME_INSTANCE->Change_Level(ETOI(m_NextLevel), LevelTitle::Create(m_Device, m_Context, m_NextLevel));
				break;
			case ETOI(LEVEL::GAMEPLAY):
				GAME_INSTANCE->Clear_Resource(ETOI(LEVEL::LOADING));
				GAME_INSTANCE->Change_Level(ETOI(m_NextLevel), LevelGamePlay::Create(m_Device, m_Context, m_NextLevel));
				break;
			default:
				break;
			}
		}
	}
}

HRESULT LevelLoading::Render_Level()
{
    m_Background->Render();
    m_PixelPanel->Render();
    m_Logo->Render();
    m_FadeOut->Render();
    m_FadeIn->Render();

    return S_OK;
}

void LevelLoading::Ready_LoadingUI()
{
    m_StaticCamera = GAME_INSTANCE->Instantiate<StaticCamera>(L"StaticCamera", ETOI(LEVEL::LOADING));
    m_Background = GAME_INSTANCE->Instantiate<LoadingBackground>(L"LoadingBackground", ETOI(LEVEL::LOADING));
    m_PixelPanel = GAME_INSTANCE->Instantiate<LoadingPixelPanel>(L"LoadingPixelPanel", ETOI(LEVEL::LOADING));
    m_Logo = GAME_INSTANCE->Instantiate<LoadingLogo>(L"LoadingLogo", ETOI(LEVEL::LOADING));

    LoadingFade::LOADING_FADE_UI_DESC fadeIn{};
    fadeIn.fadeSpeed = 0.25f;
    fadeIn.isFadeIn = true;
    fadeIn.isHuman = true;
    m_FadeIn = GAME_INSTANCE->Instantiate<LoadingFade>(L"LoadingFade", ETOI(LEVEL::LOADING), &fadeIn);
    LoadingFade::LOADING_FADE_UI_DESC fadeOut{};
    fadeOut.fadeSpeed = 0.25f;
    fadeOut.isFadeOut = true;
    fadeOut.isHuman = false;
    m_FadeOut = GAME_INSTANCE->Instantiate<LoadingFade>(L"LoadingFade", ETOI(LEVEL::LOADING), &fadeOut);
};

Shared<LevelLoading> LevelLoading::Create(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context, LEVEL nextLevelID) {
    auto loadingLevel = make_shared<LevelLoading>(device, context);

    if (FAILED(loadingLevel->Initialize(&nextLevelID))) {
		LOG_ERROR(L"Failed To Create LoadingBackground");
		return nullptr;
    }

    return loadingLevel;
}
