#include "pch.h"
#include "LevelLoading.h"

#include "Game.h"
#include "Loader.h"
#include <SpdLogger.h>
#include "LoadingFade.h"

#include "ClientSettingManager.h"
#include "LevelTitle.h"

LevelLoading::LevelLoading(const ComPtr<ID3D11Device> &device,
                           const ComPtr<ID3D11DeviceContext> &context)
    : Level{device, context}, m_Loader{nullptr}, m_NextLevel{LEVEL::LEVEL_END} {
}

HRESULT LevelLoading::Initialize(void *arg) {
    m_NextLevel = *static_cast<LEVEL*>(arg);

    if (SUCCEEDED(ClientSettingManager::GetInstance()->Sync_TextureJson_FromCSV()))
    {
        if (FAILED(ClientSettingManager::GetInstance()->Load_Textures_FromJson(LEVEL::LOADING)))
        {
            LOG_ERROR(L"Failed to load Loading Texture");
            return E_FAIL;
        }
    }

    if (FAILED(ClientSettingManager::GetInstance()->Ready_Client_Prototypes(LEVEL::LOADING)))
    {
        LOG_ERROR(L"Failed to load Loading Prototypes");
        return E_FAIL;
    }

    if (FAILED(ClientSettingManager::GetInstance()->Load_LevelData(LEVEL::LOADING)))
    {
	    LOG_ERROR(L"Failed to load Loading Level");
        return E_FAIL;
    }
    
    m_Loader = Loader::Create(m_Device, m_Context, m_NextLevel, shared_from_this(), true);
    if (nullptr == m_Loader)
      return E_FAIL;

    m_IsFinished = m_Loader->Is_Finished();
    return Level::Initialize(arg);
}

void LevelLoading::On_Destroy() { Level::On_Destroy(); }

void LevelLoading::Update_Level(Float timeDelta) {

    //if (true == m_IsFinished) {
    //    GAME_INSTANCE->Change_Level(ETOI(LEVEL::LOADING), LevelTitle::Create(m_Device, m_Context, LEVEL::TITLE));
    //}

    Level::Update_Level(timeDelta);
}

HRESULT LevelLoading::Render_Level() { return Level::Render_Level(); }

Shared<LevelLoading> LevelLoading::Create(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context, LEVEL nextLevelID) {
    auto loadingLevel = make_shared<LevelLoading>(device, context);

    if (FAILED(loadingLevel->Initialize(&nextLevelID))) {
		LOG_ERROR(L"Failed To Create LoadingBackground");
		return nullptr;
    }

    return loadingLevel;
}
