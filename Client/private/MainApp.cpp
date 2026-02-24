#include "pch.h"
#include "MainApp.h"

#include "ClientSettingManager.h"
#include "Game.h"
#include "LevelLoading.h"
#include "Loader.h"


MainApp::MainApp() : m_Game(Game::GetInstance()) {}

MainApp::~MainApp()
{
    GAME->DestroyInstance();
}

HRESULT MainApp::Initialize() {
  ENGINE_DESC EngineDesc = {};
  {
    EngineDesc.hWnd = g_hWnd;
	EngineDesc.hInst = g_hInst;
    EngineDesc.winMode = WINMODE::WIN;
    EngineDesc.levCount = ETOI(LEVEL::LOADING);

    // Use loaded project settings
    EngineDesc.viewportWidth = g_projectSettings.viewportWidth;
    EngineDesc.viewportHeight = g_projectSettings.viewportHeight;
    EngineDesc.windowTitle = g_projectSettings.windowTitle;
  }

  if (FAILED(m_Game->Initialize_Engine(EngineDesc)))
    return E_FAIL;

  // Load Layer & Tag Settings
  if (FAILED(ClientSettingManager::GetInstance()->Apply_LayerAndTagSettings())) {
		return E_FAIL;
  }

  if (FAILED(Ready_StartLevel(LEVEL::LOGO)))
    return E_FAIL;

  return S_OK;
}

void MainApp::Update() const { m_Game->Update_Engine(); }

HRESULT MainApp::Render() const {
  Shared<Float4> vClearColor = make_shared<Float4>(0.f, 0.f, 1.f, 1.f);

  if (FAILED(m_Game->Clear_BackBufferView(vClearColor)))
    return E_FAIL;

  if (FAILED(m_Game->Draw()))
    return E_FAIL;

  if (FAILED(m_Game->Present()))
    return E_FAIL;

  return S_OK;
}

HRESULT MainApp::Ready_StartLevel(LEVEL startLevel) 
{
  if (LEVEL::LOADING == startLevel)
    return E_FAIL;

  if (nullptr == Loader::Create(GAME->Get_Device(), GAME->Get_Context(), LEVEL::STATIC))
      return E_FAIL;

  if (FAILED(m_Game->Change_Level(ETOI(LEVEL::LOADING), 
      LevelLoading::Create(GAME->Get_Device(), GAME->Get_Context(), startLevel))))
  {
      return E_FAIL;
  }

  return S_OK;
}

Unique<MainApp> MainApp::Create() {
  Unique<MainApp> mainApp = make_unique<MainApp>();

  if (FAILED(mainApp->Initialize())) {

    MSG_BOX("Failed to Created : MainApp");
    return nullptr;
  }
  return mainApp;
}
