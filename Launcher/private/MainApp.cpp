#include "pch.h"
#include "MainApp.h"

#include <Game.h>
#include <ClientApp.h>
#include "ClientSettingManager.h"

namespace
{
	Bool Is_LauncherClientGate()
	{
		wchar_t value[2]{};
		return GetEnvironmentVariableW(L"NIER_PHASE6_LAUNCHER_CLIENT_GATE", value,
			static_cast<DWORD>(std::size(value))) != 0;
	}
}

MainApp::MainApp()
{
}

MainApp::~MainApp()
{
	m_ClientApp.reset();
	GAME_INSTANCE->DestroyInstance();
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
		if (!Is_LauncherClientGate())
			MSG_BOX("Failed to initialize Engine");
		return E_FAIL;
	}

	ClientSettingManager::GetInstance()->Set_ResourcePath(
		L"../../Client/bin/resources/");
	ClientSettingManager::GetInstance()->Set_ShaderPath(
		L"../../Client/bin/shaders/");

	m_ClientApp = ClientApp::Create(g_EngineDesc);
	if (!m_ClientApp)
		return E_FAIL;

	return S_OK;
}

void MainApp::Update()
{
	GAME_INSTANCE->Update_Engine();

#ifdef _DEBUG
	const Bool currF7 = (GAME_INSTANCE->Get_DIKeyState(DIK_F7) & 0x80) != 0;
	if (currF7 && !m_PrevF7)
		m_ShowFrameStats = !m_ShowFrameStats;
	m_PrevF7 = currF7;

	m_StatsTime += GAME_INSTANCE->Compute_UnscaledTimeDelta();
	++m_StatsFrames;
	if (m_StatsTime < 1.f)
		return;

	swprintf_s(m_StatsText, L"%.0f FPS  %.2f ms  |  %u draws  %.2fM tris",
		m_StatsFrames / m_StatsTime, m_StatsTime * 1000.f / m_StatsFrames,
		GAME_INSTANCE->Get_FrameDrawCount(),
		static_cast<double>(GAME_INSTANCE->Get_FrameTriangleCount()) / 1000000.0);
	m_StatsTime = 0.f;
	m_StatsFrames = 0;
#endif
}

void MainApp::Render()
{
	// Launcher는 오프스크린 없이 스왑 체인 백버퍼에 그린다. Present가 렌더 타깃을 풀기 때문에 매 프레임 다시 묶는다.
	static const Shared<Float4> clearColor = make_shared<Float4>(0.f, 0.f, 0.f, 1.f);
	GAME_INSTANCE->End_RenderOffScreen();
	GAME_INSTANCE->Clear_BackBufferView(clearColor);

	if (FAILED(GAME_INSTANCE->Draw()))
	{
		MSG_BOX("MainApp Rendering Failed");
	}

#ifdef _DEBUG
	if (m_ShowFrameStats)
		GAME_INSTANCE->Draw_Font(L"Nier_16", m_StatsText,
			Vector2(static_cast<Float>(g_EngineDesc.viewportWidth) - 520.f, 10.f));
#endif

	GAME_INSTANCE->Present();
}

Unique<MainApp> MainApp::Create()
{
	auto mainApp = make_unique<MainApp>();

	if FAILED((mainApp->Initialize()))
	{
		if (!Is_LauncherClientGate())
			MSG_BOX("Failed to Load MainApp");
		return nullptr;
	}

	return mainApp;
}
