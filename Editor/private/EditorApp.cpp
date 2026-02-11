#include "EditorApp.h"
#include "Editor_Function.h"
#include "Game.h"
#include "pch.h"

NS_BEGIN(Editor)

EditorApp::EditorApp() : m_Game(Game::GetInstance()) {}
EditorApp::~EditorApp() {
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();
}

HRESULT EditorApp::Initialize() {
  ENGINE_DESC engineDesc = {};
  {
    if (g_hWnd == nullptr) {
      MSG_BOX("g_hWnd is Invalid Pointer");
      return E_FAIL;
    }

    engineDesc.hWnd = g_hWnd;
    engineDesc.winMode = WINMODE::WIN;

    // Use loaded project settings
    engineDesc.viewportWidth = g_projectSettings.viewportWidth;
    engineDesc.viewportHeight = g_projectSettings.viewportHeight;
    engineDesc.windowTitle = g_projectSettings.windowTitle;

    // Enable Dual View (Scene + Game)
    engineDesc.useOffscreenRendering = true;
    engineDesc.renderTargetCount = 2;
  }

  if (FAILED(m_Game->Initialize_Engine(engineDesc)))
    return E_FAIL;

  // ImGui Initialize
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;           // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
  // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable
  // Multi-Viewport (Disabled to prevent Assertions) Platform Windows

  ImGui::StyleColorsDark();

  ImGuiStyle &style = ImGui::GetStyle();
  /*if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }*/

  ImGui_ImplWin32_Init(g_hWnd);
  ImGui_ImplDX11_Init(m_Game->Get_Device().Get(), m_Game->Get_Context().Get());

  return S_OK;
}

void EditorApp::Update() {
  if (nullptr == m_Game)
    return;

  m_Game->Update_Engine();
}

HRESULT EditorApp::Render() {
  if (nullptr == m_Game)
    return E_FAIL;

  HRESULT hr = {S_OK};

  // (1) Render Scene View to OffScreenRT_0
  hr = m_Game->Begin_RenderOffScreen(L"OffScreenRT_0");
  hr = m_Game->Draw();
  hr = m_Game->End_RenderOffScreen();

  hr = // (2) Render Game View to OffScreenRT_1
      hr = m_Game->Begin_RenderOffScreen(L"OffScreenRT_1");
  hr = m_Game->Draw();
  hr = m_Game->End_RenderOffScreen();

  // (3) ImGui Rendering
  // Start the ImGui frame
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  // DockSpace
  ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

  // Render Dual View Windows
  RenderDualView();

  // Rendering
  ImGui::Render();

  // Clear BackBuffer
  hr = m_Game->Clear_BackBufferView(make_shared<Float4>(
      0.1f, 0.1f, 0.1f, 1.f)); // Dark gray for Editor Background

  // Draw ImGui
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  // Update and Render additional Platform Windows
  ImGuiIO &io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
  }

  hr = m_Game->Present();

  return hr;
}

Unique<EditorApp> EditorApp::Create() {
  Unique<EditorApp> pInstance = make_unique<EditorApp>();

  if (FAILED(pInstance->Initialize())) {
    MSG_BOX("Failed to Create : EditorApp");
    return nullptr;
  }

  return pInstance;
}

void EditorApp::RenderDualView() {
  // Scene View Window
  ImGui::Begin("Scene View");
  {
    auto srv = m_Game->Get_OffScreenSRV(L"OffScreenRT_0");
    if (srv) {
      ImVec2 viewportSize = ImGui::GetContentRegionAvail();
      ImGui::Image(reinterpret_cast<ImTextureID>(srv.Get()), viewportSize);
    } else {
      ImGui::Text("Scene View: RenderTarget not available");
    }
  }
  ImGui::End();

  // Game View Window
  ImGui::Begin("Game View");
  {
    auto srv = m_Game->Get_OffScreenSRV(L"OffScreenRT_1");
    if (srv) {
      ImVec2 viewportSize = ImGui::GetContentRegionAvail();
      ImGui::Image(reinterpret_cast<ImTextureID>(srv.Get()), viewportSize);
    } else {
      ImGui::Text("Game View: RenderTarget not available");
    }
  }
  ImGui::End();
}

NS_END
