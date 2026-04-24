#include "pch.h"
#include "LauncherApp.h"

#include <Game.h>
#include <ClientApp.h>
#include "ClientSettingManager.h"

NS_BEGIN(Launcher)

LauncherApp::LauncherApp()
{
}

LauncherApp::~LauncherApp()
{
}

HRESULT LauncherApp::Initialize()
{
	ClientSettingManager::GetInstance()->Load_EngineDesc(g_EngineDesc);
	g_EngineDesc.winMode = WINMODE::FULL;
	g_EngineDesc.levelCount = ClientSettingManager::GetInstance()->Get_LevelCount();
	g_EngineDesc.startLevel = ETOI(LEVEL::TITLE);
	g_EngineDesc.hInst = g_hInst;
	g_EngineDesc.hWnd = g_hWnd;
	g_EngineDesc.viewportWidth = 1920;
	g_EngineDesc.viewportHeight = 1080;
	
	// Release 런처 전용 최적화: 백버퍼 렌더 뷰오프스크린 등 미사용, 최소한의 렌더타겟 세팅
	g_EngineDesc.useOffscreenRendering = false;
	g_EngineDesc.renderTargetCount = 0;

	if (FAILED(GAME_INSTANCE->Initialize_Engine(g_EngineDesc)))
	{
		MSG_BOX("Failed to initialize Engine in LauncherApp");
		return E_FAIL;
	}

	ClientSettingManager::GetInstance()->Set_ResourcePath(
		L"../../Client/bin/resources/");
	ClientSettingManager::GetInstance()->Set_ShaderPath(
		L"../../Client/bin/shaders/");

	m_ClientApp = Client::ClientApp::Create(g_EngineDesc);
	if (m_ClientApp == nullptr)
	{
		MSG_BOX("Failed to Create ClientApp in LauncherApp");
		return E_FAIL;
	}


	//GAME_INSTANCE->OnResize(g_EngineDesc.viewportWidth, g_EngineDesc.viewportHeight);

	m_BackgroundColor = make_shared<Color>(Color(0.f, 0.f, 0.05f, 1.f));

	return S_OK;
}

void LauncherApp::Update()
{
	GAME_INSTANCE->Update_Engine();
}

void LauncherApp::Render()
{
	if (FAILED(GAME_INSTANCE->Clear_BackBufferView(m_BackgroundColor)))
	{
		MSG_BOX("Failed to clear backBuffer");
	}

	if (FAILED(GAME_INSTANCE->Draw()))
	{
		MSG_BOX("Failed to draw");
	}

	if (FAILED(GAME_INSTANCE->Present()))
	{
		MSG_BOX("Failed to Present");
	}
}

Unique<LauncherApp> LauncherApp::Create()
{
	auto launcherApp = make_unique<LauncherApp>();

	if (FAILED(launcherApp->Initialize()))
	{
		MSG_BOX("Failed to Load LauncherApp");
		return nullptr;
	}

	return launcherApp;
}

NS_END
