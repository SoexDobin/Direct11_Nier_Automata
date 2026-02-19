#include "pch.h"
#include "EditorManager.h"
#include "Game.h"
#include "PathManager.h"

#include "EditorView.h"
#include "Inspector.h"
#include "LogConsole.h"
#include "MenuBar.h"
#include "MetadataManager.h"
#include "PrefabRegistry.h"
#include "PrefabTab.h"

IMPLEMENT_SINGLETON(EditorManager)

EditorManager::EditorManager()
    : m_Inspector{nullptr}, m_EditorView{nullptr}, m_MenuBar{nullptr},
      m_PrefabTab{nullptr}, m_LogConsole{nullptr}, m_MetadataManager{nullptr},
      m_PrefabRegistry{nullptr} {}

EditorManager::~EditorManager() {}

HRESULT EditorManager::Initialize() {
  /* MetadataManager: client_metadata.json 로드 */
  if (nullptr == (m_MetadataManager = MetadataManager::Create()))
    return E_FAIL;
  m_MetadataManager->LoadMetadata(PATH.GetClientMetadataPath());

  /* PrefabRegistry: ProjectSetting/Prefab/ 로드 */
  if (nullptr == (m_PrefabRegistry = PrefabRegistry::Create()))
    return E_FAIL;
  m_PrefabRegistry->LoadFromDir(PATH.GetPrefabSettingsDir());

  /* UI 윈도우 */
  if (nullptr == (m_Inspector = Inspector::Create()))
    return E_FAIL;
  if (nullptr == (m_EditorView = EditorView::Create()))
    return E_FAIL;
  if (nullptr == (m_MenuBar = MenuBar::Create()))
    return E_FAIL;
  if (nullptr == (m_PrefabTab = PrefabTab::Create()))
    return E_FAIL;
  if (nullptr == (m_LogConsole = LogConsole::Create()))
    return E_FAIL;

  m_PrefabTab->Set_Inspector(m_Inspector);

  return S_OK;
}

void EditorManager::Update() {
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  m_Inspector->Update();
  m_EditorView->Update();
  m_MenuBar->Update();
  m_PrefabTab->Update();
  m_LogConsole->Update();
}

void EditorManager::Render() {
  if (FAILED(GAME->Clear_BackBufferView(
          make_shared<Float4>(0.1f, 0.1f, 0.1f, 1.f)))) {
    LOG_CRITICAL(L"Failed To Clear Back Buffer");
    return;
  }

  ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

  m_Inspector->Render();
  m_EditorView->Render();
  m_MenuBar->Render();
  m_PrefabTab->Render();
  m_LogConsole->Render();

  if (FAILED(GAME->End_RenderOffScreen())) {
    LOG_CRITICAL(L"Failed To RTV Off");
    return;
  }
  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  ImGuiIO &io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
  }

  if (FAILED(GAME->Present())) {
    LOG_CRITICAL(L"Failed To Present SwapChain");
    return;
  }
}
