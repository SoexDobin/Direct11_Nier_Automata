#include "pch.h"
#include "EditorManager.h"
#include "Game.h"
#include "PathManager.h"

#include "EditorView.h"
#include "Inspector.h"
#include "LogConsole.h"
#include "MenuBar.h"

IMPLEMENT_SINGLETON(EditorManager)

EditorManager::EditorManager()
    : m_Inspector{nullptr}, m_EditorView{nullptr}, m_MenuBar{nullptr},
      m_PrefabTab{nullptr}, m_LogConsole{nullptr}
{}

EditorManager::~EditorManager() {}

HRESULT EditorManager::Initialize() 
{
    /* UI 윈도우 */
    if (nullptr == (m_Inspector = Inspector::Create()))
		return E_FAIL;
    if (nullptr == (m_EditorView = EditorView::Create()))
		return E_FAIL;
    if (nullptr == (m_MenuBar = MenuBar::Create()))
		return E_FAIL;
    if (nullptr == (m_LogConsole = LogConsole::Create()))
		return E_FAIL;

  return S_OK;
}

void EditorManager::Update() {
    m_Inspector->Update();
    m_EditorView->Update();
    m_MenuBar->Update();
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
