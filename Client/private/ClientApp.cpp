#include "pch.h"
#include "ClientApp.h"
#include "Client_Function.h"

#include "ClientSettingManager.h"
#include "Game.h"
#include "LevelLoading.h"
#include <mutex>

#include "LoadingFade.h"
#include "SpdLogger.h"
#include "StaticCamera.h"

ClientApp::ClientApp() {}

ClientApp::~ClientApp() {}

HRESULT ClientApp::Initialize(const ENGINE_DESC& desc) 
{
    Client::Register_Client_Reflection();

    ClientSettingManager::g_EngineDesc = desc;

    if (FAILED(ClientSettingManager::GetInstance()->Apply_LayerAndTagSettings())) {
      return E_FAIL;
    }
    if (FAILED(ClientSettingManager::GetInstance()->Load_Shader()))
        return E_FAIL;
    if (FAILED(Ready_InitialObject()))
        return E_FAIL;

    if (FAILED(Ready_StartLevel(static_cast<LEVEL>(ClientSettingManager::g_EngineDesc.startLevel))))
      return E_FAIL;

    return S_OK;
}

HRESULT ClientApp::Ready_StartLevel(LEVEL startLevel) 
{
	if (FAILED(GAME_INSTANCE->Change_Level(ETOI(LEVEL::LOADING), LevelLoading::Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context(), startLevel, true)))) {
		return E_FAIL;
    }

    return S_OK;
}

HRESULT ClientApp::Ready_InitialObject()
{
    GAME_INSTANCE->Add_Prototype(ETOI(LEVEL::STATIC), StaticCamera::Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context()), L"StaticCamera");
    GAME_INSTANCE->Add_Prototype(ETOI(LEVEL::STATIC), LoadingFade::Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context()), L"LoadingFade");

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
