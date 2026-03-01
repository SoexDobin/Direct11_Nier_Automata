#include "EditorApp.h"
#include "ClientApp.h"
#include "ClientSettingManager.h"
#include "pch.h"

#include "EditorManager.h"

#include "LayerRegistry.h"
#include "PathManager.h"
#include "TagRegistry.h"

EditorApp::EditorApp() {}
EditorApp::~EditorApp() { Destruct_IMGUI(); }

HRESULT EditorApp::Initialize() {
  ENGINE_DESC desc = {};
  {
    desc.hWnd = g_hWnd;
    desc.hInst = g_hInst;
    desc.winMode = WINMODE::WIN;
    desc.viewportWidth = g_projectSettings.viewportWidth;
    desc.viewportHeight = g_projectSettings.viewportHeight;
    desc.windowTitle = g_projectSettings.windowTitle;
    desc.levelCount = ClientSettingManager::GetInstance()->Get_LevelCount();
    desc.startLevel = 1;
    desc.useOffscreenRendering = true;
    desc.renderTargetCount = 1;
  }

  /* 2. 엔진 Core 설정 1순위 그래야만 하고 그래야하게 만들어야 함. 여기서
   * PrototypeManager가 호출됩니다. */
  if (FAILED(GAME_INSTANCE->Initialize_Engine(desc)))
    return E_FAIL;

  m_EngineDesc = desc;

  if ((m_ClientApp = ClientApp::Create(desc))) {

  } else {
    MSG_BOX("Failed To Create : ClientApp");
    return E_FAIL;
  }

  if (FAILED(EDITOR->Initialize()))
    return E_FAIL;
  if (FAILED(Initialize_IMGUI(desc)))
    return E_FAIL;

  return S_OK;
}

void EditorApp::Update() {
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  Bool isPlaying = EDITOR->Is_PlayMode();

  if (m_WasPlaying && !isPlaying) {
    Reset_ClientApp();
  }
  m_WasPlaying = isPlaying;

  EDITOR->Update();

  if (isPlaying) {
    GAME_INSTANCE->Update_Engine();
  }
}

HRESULT EditorApp::Render() {
  Shared<Float4> vClearColor = make_shared<Float4>(0.f, 0.f, 1.f, 1.f);
  if (FAILED(GAME_INSTANCE->Clear_BackBufferView(vClearColor)))
    return E_FAIL;

  // TODO : 게임 카메라 세팅

  if (FAILED(GAME_INSTANCE->Begin_RenderOffScreen(0)))
    return E_FAIL;

  // TODO : 에디터 카메라 세팅

  if (FAILED(GAME_INSTANCE->Draw()))
    return E_FAIL;

  if (FAILED(GAME_INSTANCE->End_RenderOffScreen()))
    return E_FAIL;

  EDITOR->Render();

  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  ImGuiIO &io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
  }

  return GAME_INSTANCE->Present();
}

Unique<EditorApp> EditorApp::Create() {
  Unique<EditorApp> editorApp = make_unique<EditorApp>();

  if (FAILED(editorApp->Initialize())) {
    MSG_BOX("Failed to Create : EditorApp");
    return nullptr;
  }

  return editorApp;
}

HRESULT EditorApp::Initialize_IMGUI(const ENGINE_DESC &desc) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;           // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport /
                                                      // Platform Windows

  ImGui::StyleColorsDark();
  ImGuiStyle &style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  ImGui_ImplWin32_Init(desc.hWnd);
  if (GAME_INSTANCE->Get_Device().Get() == nullptr ||
      GAME_INSTANCE->Get_Context().Get() == nullptr) {
    MSG_BOX("Failed ImGui_ImplDX11_Init");
    return E_FAIL;
  }
  if (!ImGui_ImplDX11_Init(GAME_INSTANCE->Get_Device().Get(),
                           GAME_INSTANCE->Get_Context().Get())) {
    MSG_BOX("Failed ImGui_ImplDX11_Init");
    return E_FAIL;
  }

  {
    RECT rc{};
    GetClientRect(g_hWnd, &rc);
    uint32 width = rc.right - rc.left;
    uint32 height = rc.bottom - rc.top;
    GAME_INSTANCE->OnResize(width, height);
  }
  return S_OK;
}

HRESULT EditorApp::Destruct_IMGUI() {
  GAME_INSTANCE->Get_LayerRegister()->SaveToFile(PATH.GetLayerSettingsPath());
  GAME_INSTANCE->Get_TagRegister()->SaveToFile(PATH.GetTagSettingsPath());

  GAME_INSTANCE->DestroyInstance();

  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  return S_OK;
}

void EditorApp::Reset_ClientApp() {
  m_ClientApp.reset();
  GAME_INSTANCE->Clear_AllResource();
  m_ClientApp = ClientApp::Create(m_EngineDesc);
}
