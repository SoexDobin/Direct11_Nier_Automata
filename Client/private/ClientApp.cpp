#include "pch.h"
#include "ClientApp.h"
#include "Client_Function.h"

#include "ClientSettingManager.h"
#include "Game.h"
#include "LevelLoading.h"
#include <mutex>

#include "SpdLogger.h"

ClientApp::ClientApp() {}

ClientApp::~ClientApp() {}

HRESULT ClientApp::Initialize(const ENGINE_DESC &desc) {
    Client::Register_Client_Reflection();

    if (FAILED(ClientSettingManager::GetInstance()->Apply_LayerAndTagSettings())) {
      return E_FAIL;
    }
    if (FAILED(ClientSettingManager::GetInstance()->Load_EngineDesc(ClientSettingManager::g_EngineDesc)))
        return E_FAIL;

    if (FAILED(ClientSettingManager::GetInstance()->Load_Shader()))
        return E_FAIL;
    if (SUCCEEDED(ClientSettingManager::GetInstance()->Sync_TextureJson_FromCSV())) {
        if (FAILED(ClientSettingManager::GetInstance()->Load_Textures_FromJson(LEVEL::STATIC))) {
            LOG_ERROR(L"Failed to Load Textures");
            return E_FAIL;
        }
    }
    if (SUCCEEDED(ClientSettingManager::GetInstance()->Sync_ModelJson_FromCSV())) {
	    if (FAILED(ClientSettingManager::GetInstance()->Load_Model_FromJson(LEVEL::STATIC))) {
            LOG_ERROR(L"Failed to Load Model");
            return E_FAIL;
	    }
    }

    if (FAILED(ClientSettingManager::GetInstance()->Ready_Client_Prototypes(LEVEL::STATIC))) {
        LOG_ERROR(L"Failed to Ready Client Prototypes");
        return E_FAIL;
    }
    if (FAILED(ClientSettingManager::GetInstance()->Ready_Client_Prototypes(LEVEL::LOGO))) {
        LOG_ERROR(L"Failed to Ready Client Prototypes");
        return E_FAIL;
    }
    if (FAILED(ClientSettingManager::GetInstance()->Ready_Client_Prototypes(LEVEL::LOADING))) {
        LOG_ERROR(L"Failed to Ready Client Prototypes");
        return E_FAIL;
    }

    if (FAILED(Ready_StartLevel(LEVEL::LOGO)))
      return E_FAIL;

    return S_OK;
}

HRESULT ClientApp::Ready_StartLevel(LEVEL startLevel) {
  if (LEVEL::LOADING == startLevel)
    return E_FAIL;

  if (FAILED(GAME_INSTANCE->Change_Level(
          ETOI(LEVEL::LOGO),
          LevelLoading::Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context(), startLevel)))) {
    return E_FAIL;
  }

  return S_OK;
}

Unique<ClientApp> ClientApp::Create(const ENGINE_DESC &desc) {
  Unique<ClientApp> mainApp = make_unique<ClientApp>();

  if (FAILED(mainApp->Initialize(desc))) {

    MSG_BOX("Failed to Created : ClientApp");
    return nullptr;
  }
  return mainApp;
}
