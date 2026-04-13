#include "pch.h"
#include "ClientApp.h"
#include "Client_Function.h"

#include "ClientSettingManager.h"
#include "Game.h"
#include "LevelLoading.h"
#include <mutex>

#include "LoadingFadeIn.h"
#include "LoadingFadeOut.h"
#include "SpdLogger.h"
#include "StaticCamera.h"
#include "VISphere.h"
#include "SkySphere.h"

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
    if (FAILED(ClientSettingManager::GetInstance()->Load_Navigation_FromBinary()))
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
    GAME_INSTANCE->Add_Font(L"Nier_16", L"../../Client/bin/resources/Font/NierFont_16.spritefont");
    GAME_INSTANCE->Add_Font(L"Nier_32", L"../../Client/bin/resources/Font/NierFont_32.spritefont");
    GAME_INSTANCE->Add_Font(L"Nier_64", L"../../Client/bin/resources/Font/NierFont_64.spritefont");

    GAME_INSTANCE->Add_Prototype(ETOI(LEVEL::STATIC), StaticCamera::Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context()), L"StaticCamera");
    GAME_INSTANCE->Add_Prototype(ETOI(LEVEL::STATIC), LoadingFadeIn::Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context()), L"LoadingFadeIn");
    GAME_INSTANCE->Add_Prototype(ETOI(LEVEL::STATIC), LoadingFadeOut::Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context()), L"LoadingFadeOut");
    GAME_INSTANCE->Add_Prototype(ETOI(LEVEL::STATIC), VISphere::Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context()), L"VISphere");
    GAME_INSTANCE->Add_Prototype(ETOI(LEVEL::STATIC), SkySphere::Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context()), L"SkySphere");

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
