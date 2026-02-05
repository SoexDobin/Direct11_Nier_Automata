#include "pch.h"
#include "MainApp.h"

#include "Game.h"

MainApp::MainApp()
	: m_Game( Game::GetInstance() )
{
}

HRESULT MainApp::Initialize()
{
	ENGINE_DESC		EngineDesc = {};
	{
		EngineDesc.hWnd = g_hWnd;
		EngineDesc.winMode = WINMODE::WIN;
		EngineDesc.levCount = ETOI(LEVEL::END);
		EngineDesc.viewportWidth = g_WinSizeX;
		EngineDesc.viewportHeight = g_WinSizeY;
	}
	

	if (FAILED(m_Game->Initialize_Engine(EngineDesc, m_Device, m_Context)))
		return E_FAIL;

	/* 내 게임의 시작을 위해 시작이되는 레벨 할당과 동작을 시킨다. */
	if (FAILED(Ready_StartLevel(LEVEL::LOGO)))
		return E_FAIL;

	return S_OK;
}

void MainApp::Update() const
{
	m_Game->Update_Engine();
}

HRESULT MainApp::Render() const
{
	return m_Game->Draw();
}

HRESULT MainApp::Ready_StartLevel(LEVEL startLevel)
{
	if (LEVEL::LOADING == startLevel)
		return E_FAIL;


	/*if (FAILED(m_Game->Change_Level(ETOI(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevelID))))
		return E_FAIL;*/

	return S_OK;
}

Unique<MainApp> MainApp::Create()
{
	return make_unique<MainApp>();
}
