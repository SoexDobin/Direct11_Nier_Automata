#include "pch.h"
#include "ClientApp.h"

#include "ClientSettingManager.h"
#include "Game.h"
#include "LevelLoading.h"
#include "Loader.h"


ClientApp::ClientApp() {}

ClientApp::~ClientApp()
{
    
}

HRESULT ClientApp::Initialize(const ENGINE_DESC& desc) 
{
	if (FAILED(GAME->Initialize_Engine(desc)))
		return E_FAIL;

	// Load Layer & Tag Settings
	if (FAILED(ClientSettingManager::GetInstance()->Apply_LayerAndTagSettings())) {
		return E_FAIL;
	}

	if (FAILED(Ready_StartLevel(LEVEL::LOGO)))
		return E_FAIL;

	return S_OK;
}

void ClientApp::Update() const { GAME->Update_Engine(); }

HRESULT ClientApp::Render() const {
  Shared<Float4> vClearColor = make_shared<Float4>(0.f, 0.f, 1.f, 1.f);

  if (FAILED(GAME->Clear_BackBufferView(vClearColor)))
    return E_FAIL;

  if (FAILED(GAME->Draw()))
    return E_FAIL;

  if (FAILED(GAME->Present()))
    return E_FAIL;

  return S_OK;
}

HRESULT ClientApp::Ready_StartLevel(LEVEL startLevel) 
{
  if (LEVEL::LOADING == startLevel)
    return E_FAIL;

  if (nullptr == Loader::Create(GAME->Get_Device(), GAME->Get_Context(), LEVEL::STATIC))
      return E_FAIL;

  if (FAILED(GAME->Change_Level(ETOI(LEVEL::LOADING),
      LevelLoading::Create(GAME->Get_Device(), GAME->Get_Context(), startLevel))))
  {
      return E_FAIL;
  }

  return S_OK;
}

Unique<ClientApp> ClientApp::Create(const ENGINE_DESC& desc) {
  Unique<ClientApp> mainApp = make_unique<ClientApp>();

  if (FAILED(mainApp->Initialize(desc))) {

    MSG_BOX("Failed to Created : ClientApp");
    return nullptr;
  }
  return mainApp;
}
