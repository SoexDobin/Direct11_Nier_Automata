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
	ClientSettingManager::GetInstance()->Load_EngineDesc(g_EngineDecs);
	g_EngineDecs.winMode = WINMODE::WIN;
	g_EngineDecs.levelCount = ClientSettingManager::GetInstance()->Get_LevelCount();
	g_EngineDecs.startLevel = ETOI(LEVEL::TITLE);
	g_EngineDecs.useOffscreenRendering = false;
	g_EngineDecs.renderTargetCount = 0;

	if (FAILED(GAME_INSTANCE->Initialize_Engine(g_EngineDecs)))
	{
		MSG_BOX("Failed to initialize Engine");
		return E_FAIL;
	}

	ClientSettingManager::GetInstance()->Set_ResourcePath(
		L"../../Client/bin/resources/");
	ClientSettingManager::GetInstance()->Set_ShaderPath(
		L"../../Client/bin/shaders/");

	m_ClientApp = ClientApp::Create(g_EngineDecs);

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
