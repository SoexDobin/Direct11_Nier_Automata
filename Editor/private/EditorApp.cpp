#include "EditorApp.h"
#include "pch.h"

#include "EditorManager.h"
#include "Game.h"
#include "PathManager.h"
#include "PrefabRegistry.h"

EditorApp::EditorApp() : m_Game{GAME}, m_Editor{EDITOR} {}
EditorApp::~EditorApp() {
  Destruct_IMGUI();

  GAME->DestroyInstance();
  EDITOR->DestroyInstance();
}

HRESULT EditorApp::Initialize() {
  ENGINE_DESC engineDesc = {};
  {
    engineDesc.hWnd = g_hWnd;
    engineDesc.winMode = WINMODE::WIN;
    engineDesc.viewportWidth = g_projectSettings.viewportWidth;
    engineDesc.viewportHeight = g_projectSettings.viewportHeight;
    engineDesc.windowTitle = g_projectSettings.windowTitle;
    engineDesc.useOffscreenRendering = true;
    engineDesc.renderTargetCount = 2;
  }

  if (FAILED(GAME->Initialize_Engine(engineDesc)))
    return E_FAIL;
  if (FAILED(EDITOR->Initialize()))
    return E_FAIL;

  if (FAILED(Initialize_IMGUI()))
    return E_FAIL;

  return S_OK;
}

void EditorApp::Update() {
  GAME->Update_Engine();
  EDITOR->Update();
}

HRESULT EditorApp::Render() {
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

HRESULT EditorApp::Initialize_IMGUI() 
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;           // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking

  io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/malgun.ttf", 16.0f, nullptr,
                               io.Fonts->GetGlyphRangesKorean());

  ImGui::StyleColorsDark();
  ImGuiStyle &style = ImGui::GetStyle();

  ImGui_ImplWin32_Init(g_hWnd);
  ImGui_ImplDX11_Init(GAME->Get_Device().Get(), GAME->Get_Context().Get());

  RECT rc{};
  GetClientRect(g_hWnd, &rc);
  uint32 width = rc.right - rc.left;
  uint32 height = rc.bottom - rc.top;
  GAME->OnResize(width, height);
  //io.ConfigWindowsResizeFromEdges();

  return S_OK;
}

HRESULT EditorApp::Destruct_IMGUI() {
  GAME->Get_LayerRegister()->SaveToFile(PATH.GetLayerSettingsPath());
  GAME->Get_TagRegister()->SaveToFile(PATH.GetTagSettingsPath());

  /* Prefab 오버라이드 저장 */
  EDITOR->Get_PrefabRegistry()->SaveToDir(PATH.GetPrefabSettingsDir());

  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  return S_OK;
}
