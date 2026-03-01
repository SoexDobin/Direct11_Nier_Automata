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
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground; // 배경 투명화
    // 4. (선택 사항) 만약 화면 윗부분에 메뉴바(File, Edit 등)를 넣고 싶다면 이 속성 추가
    // window_flags |= ImGuiWindowFlags_MenuBar;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Main Panel", nullptr, window_flags);
    {
        ImGui::PopStyleVar(3);
        // 7. 이 패널 공간 전체를 "도킹 가능한 구역(DockSpace)"으로 만듦
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
        }
    }
    ImGui::End();

    m_Inspector->Render();
    m_EditorView->Render();
    m_MenuBar->Render();
    m_LogConsole->Render();
}
