#include "pch.h"
#include "MainApp.h"

#include <Game.h>
#include <ClientApp.h>
#include "ClientSettingManager.h"

MainApp::MainApp()
{
}

MainApp::~MainApp()
{
}

HRESULT MainApp::Initialize()
{
	ClientSettingManager::GetInstance()->Load_EngineDesc(g_EngineDesc);

#ifdef _DEBUG
	g_EngineDesc.winMode = WINMODE::WIN;
	if (g_EngineDesc.viewportWidth > 1600u)
		g_EngineDesc.viewportWidth = 1600u;
	if (g_EngineDesc.viewportHeight > 900u)
		g_EngineDesc.viewportHeight = 900u;

	const LONG_PTR exStyle = GetWindowLongPtr(g_hWnd, GWL_EXSTYLE) & ~WS_EX_TOPMOST;
	SetWindowLongPtr(g_hWnd, GWL_EXSTYLE, exStyle);

	RECT windowRect{0, 0, static_cast<LONG>(g_EngineDesc.viewportWidth),
	                static_cast<LONG>(g_EngineDesc.viewportHeight)};
	AdjustWindowRect(&windowRect,
	                 static_cast<DWORD>(GetWindowLongPtr(g_hWnd, GWL_STYLE)),
	                 FALSE);
	SetWindowPos(g_hWnd, HWND_NOTOPMOST, 64, 64,
	             windowRect.right - windowRect.left,
	             windowRect.bottom - windowRect.top, SWP_SHOWWINDOW);
#endif

	g_EngineDesc.levelCount = ClientSettingManager::GetInstance()->Get_LevelCount();
	g_EngineDesc.startLevel = ETOI(LEVEL::TITLE);
	g_EngineDesc.useOffscreenRendering = false;
	g_EngineDesc.renderTargetCount = 0;

	if (FAILED(GAME_INSTANCE->Initialize_Engine(g_EngineDesc)))
	{
		MSG_BOX("Failed to initialize Engine");
		return E_FAIL;
	}

	ClientSettingManager::GetInstance()->Set_ResourcePath(
		L"../../Client/bin/resources/");
	ClientSettingManager::GetInstance()->Set_ShaderPath(
		L"../../Client/bin/shaders/");

	m_ClientApp = ClientApp::Create(g_EngineDesc);

	return S_OK;
}

void MainApp::Update()
{
	GAME_INSTANCE->Update_Engine();
}

void MainApp::Render()
{
	if (FAILED(GAME_INSTANCE->Draw()))
	{
		MSG_BOX("MainApp Rendering Failed");
	}
}

Unique<MainApp> MainApp::Create()
{
	auto mainApp = make_unique<MainApp>();

	if FAILED((mainApp->Initialize()))
	{
		MSG_BOX("Failed to Load MainApp");
		return nullptr;
	}

	return mainApp;
}
