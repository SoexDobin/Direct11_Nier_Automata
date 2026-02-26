#include "pch.h"
#include "EditorApp.h"
#include "ClientApp.h"

#include "EditorManager.h"

#include "PathManager.h"
#include "PrefabRegistry.h"
#include "LayerRegistry.h"
#include "TagRegistry.h"

EditorApp::EditorApp() {}
EditorApp::~EditorApp() {
  Destruct_IMGUI();

  GAME->DestroyInstance();
  EDITOR->DestroyInstance();
}

HRESULT EditorApp::Initialize() {
  ENGINE_DESC desc = {};
  {
    desc.hWnd = g_hWnd;
	desc.hInst = g_hInst;
    desc.winMode = WINMODE::WIN;
    desc.viewportWidth = g_projectSettings.viewportWidth;
    desc.viewportHeight = g_projectSettings.viewportHeight;
    desc.windowTitle = g_projectSettings.windowTitle;
    desc.useOffscreenRendering = true;
    desc.renderTargetCount = 2;
  }

  if (FAILED(GAME->Initialize_Engine(desc)))
    return E_FAIL;
  if (FAILED(EDITOR->Initialize()))
    return E_FAIL;

  if (FAILED(Initialize_IMGUI(desc)))
    return E_FAIL;

  return S_OK;
}

void EditorApp::Update() 
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

	GAME->Update_Engine();
	EDITOR->Update();
}

HRESULT EditorApp::Render() 
{
	EDITOR->Render();
	return S_OK;
}

Unique<EditorApp> EditorApp::Create() {
	Unique<EditorApp> editorApp = make_unique<EditorApp>();

    if (FAILED(editorApp->Initialize())) {
        MSG_BOX("Failed to Create : EditorApp");
        return nullptr;
    }

  return editorApp;
}

HRESULT EditorApp::Initialize_IMGUI(const ENGINE_DESC& desc) 
{
    IMGUI_CHECKVERSION();
	ImGui::CreateContext();

    ImGuiIO io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

    ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplWin32_Init(desc.hWnd);
    ImGui_ImplDX11_Init(GAME->Get_Device().Get(), GAME->Get_Context().Get());

    {
        RECT rc{};
        GetClientRect(g_hWnd, &rc);
        uint32 width = rc.right - rc.left;
        uint32 height = rc.bottom - rc.top;
        GAME->OnResize(width, height);
    }
	return S_OK;
}

HRESULT EditorApp::Destruct_IMGUI() {
  GAME->Get_LayerRegister()->SaveToFile(PATH.GetLayerSettingsPath());
  GAME->Get_TagRegister()->SaveToFile(PATH.GetTagSettingsPath());

  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  return S_OK;
}
