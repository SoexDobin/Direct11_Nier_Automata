#include "pch.h"
#include "ImGuizmo.h"
#include "EditorApp.h"
#include "ClientApp.h"
#include "ClientSettingManager.h"

#include "EditorManager.h"

#include "LayerRegistry.h"
#include "PathManager.h"
#include "TagRegistry.h"

EditorApp::EditorApp() {}
EditorApp::~EditorApp()
{
	Destruct_IMGUI();
}

HRESULT EditorApp::Initialize() {
  m_Game = GAME_INSTANCE;

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
    desc.renderTargetCount = 2;
  }

  /* 엔진 Core 설정 1순위 그래야만 하고 그래야하게 만들어야 함. 여기서
   * PrototypeManager가 호출됩니다. */
  if (FAILED(GAME_INSTANCE->Initialize_Engine(desc)))
    return E_FAIL;

  ClientSettingManager::GetInstance()->Set_ResourcePath(
      L"../../Client/bin/resources/");
  ClientSettingManager::GetInstance()->Set_ShaderPath(
      L"../../Client/bin/shaders/");

  m_EngineDesc = desc;

  if ((m_ClientApp = ClientApp::Create(desc))) {

  } else {
    MSG_BOX("Failed To Create : ClientApp");
    return E_FAIL;
  }

  if (FAILED(Initialize_IMGUI(desc)))
      return E_FAIL;

  if (FAILED(EDITOR->Initialize()))
    return E_FAIL;
  
  GAME_INSTANCE->OnResize(desc.viewportWidth, desc.viewportHeight, 0);
  GAME_INSTANCE->OnResize(desc.viewportWidth, desc.viewportHeight, 1);

  return S_OK;
}

void EditorApp::Update() {
    static EDITOR_STATE prevState = EDITOR_STATE::STOP;
    EDITOR_STATE curState = EDITOR->Get_State();
    
    if (curState == EDITOR_STATE::STOP && prevState != EDITOR_STATE::STOP) {
        m_IsReset = true;
    }
    prevState = curState;
    
    if (m_IsReset) {
        Reset_ClientApp();
    }
    
    if (EDITOR->Is_ResizeRequest()) {
        EditorManager::RESIZE_INFO info = EDITOR->Get_ResizeInfo();
        GAME_INSTANCE->OnResize(static_cast<uint32>(info.width), static_cast<uint32>(info.height), info.screenIndex);
    }
    
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
    
    EDITOR->Update(m_IsReset);
}

HRESULT EditorApp::Render() {
    EDITOR->Render(m_IsReset);
    
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
    
    if (m_IsReset) {
        m_IsReset = false;
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

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;           // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport /
                                                        // Platform Windows

    ImGui::StyleColorsDark();
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      style.WindowRounding = 0.f;
      style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    if (desc.hWnd == nullptr) {
        MSG_BOX("ERROR: HWND NULL! Failed ton Initialize ImGui");
        return E_FAIL;
    }

    ImGui_ImplWin32_Init(desc.hWnd);
    if (GAME_INSTANCE->Get_Device().Get() == nullptr ||
        GAME_INSTANCE->Get_Context().Get() == nullptr) {
      MSG_BOX("Failed ImGui_ImplDX11_Init");
      return E_FAIL;
    }
    if (!ImGui_ImplDX11_Init(GAME_INSTANCE->Get_Device().Get(), GAME_INSTANCE->Get_Context().Get())) {
      MSG_BOX("Failed ImGui_ImplDX11_Init");
      return E_FAIL;
    }

    return S_OK;
}

HRESULT EditorApp::Destruct_IMGUI() {
    GAME_INSTANCE->Get_LayerRegister()->SaveToFile(PATH.GetLayerSettingsPath());
    GAME_INSTANCE->Get_TagRegister()->SaveToFile(PATH.GetTagSettingsPath());

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    
    m_ClientApp.reset();
    m_Game.reset();
    GAME_INSTANCE->DestroyInstance();

	return S_OK;
}

void EditorApp::Reset_ClientApp() {
  m_ClientApp.reset();
  GAME_INSTANCE->Clear_AllResource();
  m_ClientApp = ClientApp::Create(m_EngineDesc);
}
