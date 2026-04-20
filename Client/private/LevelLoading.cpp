#include "pch.h"
#include "LevelLoading.h"

#include "Game.h"
#include "Loader.h"
#include <SpdLogger.h>
#include "LoadingFadeIn.h"
#include "LoadingFadeOut.h"

#include "ClientSettingManager.h"
#include "LevelGamePlay.h"
#include "LevelGamePlay2.h"
#include "LevelTitle.h"
#include "LoadingBackground.h"
#include "LoadingLogo.h"
#include "LoadingPixelPanel.h"
#include "StaticCamera.h"

LevelLoading::LevelLoading(const ComPtr<ID3D11Device> &device,
                           const ComPtr<ID3D11DeviceContext> &context)
    : Level{device, context}, m_Loader{nullptr}, m_NextLevel{LEVEL::LEVEL_END} {
}

LevelLoading::~LevelLoading()
{
    Destroy(m_StaticCamera);
}

HRESULT LevelLoading::Initialize(void *arg) {
    LEVEL_LOADING_DESC desc = *static_cast<LEVEL_LOADING_DESC*>(arg);
    m_NextLevel = desc.nextLevelID;
    m_IsLoadStatic = desc.loadStatic;
    GAME_INSTANCE->Set_TargetLevelIndex(ETOI(m_NextLevel));

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

    m_Loader = Loader::Create(m_Device, m_Context, m_NextLevel, shared_from_this(), m_IsLoadStatic);
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

    if (!m_FadeOut->Is_FadeFinished())
    {
        m_FadeOut->Update(timeDelta);
    }

    if (true == m_IsFinished && m_FadeOut->Is_FadeFinished())
    {
        Transition_To_NextLevel(timeDelta);
    }
}

void LevelLoading::Update_LoadLevel(Float timeDelta)
{
    m_Background->Update(timeDelta);
    m_PixelPanel->Update(timeDelta);
    m_Logo->Update(timeDelta);
    
    if (!m_FadeOut->Is_FadeFinished())
    {
        m_FadeOut->Update(timeDelta);
    }
    if (true == m_IsFinished && m_FadeOut->Is_FadeFinished())
    {
        Transition_To_NextLevel(timeDelta);
    }
}

void LevelLoading::Transition_To_NextLevel(Float timeDelta)
{
	if (!m_FadeIn->Is_FadeFinished())
	{
		m_FadeIn->Set_Active(true);
		m_FadeIn->Update(timeDelta); 
	}
	else
	{
		if (ClientSettingManager::GetInstance()->AutoTransitionLevel(LEVEL::LOADING, m_NextLevel))
		{
			switch (ETOI(m_NextLevel))
			{
			case ETOI(LEVEL::TITLE):
				GAME_INSTANCE->Change_Level(ETOI(m_NextLevel), LevelTitle::Create(m_Device, m_Context));
				break;
			case ETOI(LEVEL::GAMEPLAY):
				GAME_INSTANCE->Change_Level(ETOI(m_NextLevel), LevelGamePlay::Create(m_Device, m_Context));
				break;
            case ETOI(LEVEL::GAMEPLAY2):
                GAME_INSTANCE->Change_Level(ETOI(m_NextLevel), LevelGamePlay2::Create(m_Device, m_Context));
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

    LoadingFadeIn::FADE_IN_DESC fadeIn{};
    fadeIn.fadeSpeed = 0.25f;
    m_FadeIn = GAME_INSTANCE->Instantiate<LoadingFadeIn>(L"LoadingFadeIn", ETOI(LEVEL::LOADING), &fadeIn);

    LoadingFadeOut::FADE_OUT_DESC fadeOut{};
    fadeOut.fadeSpeed = 0.25f;
    m_FadeOut = GAME_INSTANCE->Instantiate<LoadingFadeOut>(L"LoadingFadeOut", ETOI(LEVEL::LOADING), &fadeOut);
};

Shared<LevelLoading> LevelLoading::Create(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context, LEVEL nextLevelID, Bool loadStatic) {
    auto loadingLevel = make_shared<LevelLoading>(device, context);

    LEVEL_LOADING_DESC desc{};
    desc.nextLevelID = nextLevelID;
    desc.loadStatic = loadStatic;

    if (FAILED(loadingLevel->Initialize(&desc))) {
		LOG_ERROR(L"Failed To Create LoadingBackground");
		return nullptr;
    }

    return loadingLevel;
}
