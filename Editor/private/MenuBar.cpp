#include "pch.h"
#include "MenuBar.h"
#include "PathManager.h"
#include "EditorManager.h"
#include "ModelViewer.h"
#include "ClientSettingManager.h"
#include "ModelViewer.h"



MenuBar::MenuBar() {}

HRESULT MenuBar::Initialize() {
    m_Enable = false;
    m_Game = GAME_INSTANCE;
    return EditorObject::Initialize();
}
void MenuBar::Update(Bool isResize) { EditorObject::Update(isResize); }
void MenuBar::Render(Bool isResize) {
  EditorObject::Render(isResize);
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("WindowSetting")) {
      ImGui::Separator();

      Bool showModelViewer = EDITOR->Get_ModelViewer()->Is_Enabled();
      if (ImGui::MenuItem("Model Viewer", nullptr, showModelViewer))
          EDITOR->Get_ModelViewer()->Set_Enable(!showModelViewer);

      ImGui::EndMenu();
    }

    // ── SceneData 메뉴 ─────────────────────────────────────────────────
    if (ImGui::BeginMenu("LevelData"))
    {
      Bool isStop = (EDITOR->Get_State() == EDITOR_STATE::STOP);

      uint32 displayIndex = m_Game->Get_CurrentLevelIndex();
      if (displayIndex == 1) // 로딩 중이면 대상 레벨 표시
          displayIndex = EDITOR->Get_EngineDesc().startLevel;

      string saveLabel = "Save (" + Helper::To_String(std::filesystem::path(PATH.GetLevelDataPath(displayIndex)).filename().wstring()) + ")";
      if (ImGui::MenuItem(saveLabel.c_str(), nullptr, false, isStop))
      {
        if (SUCCEEDED(m_Game->SerializeLevel(PATH.GetLevelDataPath(displayIndex))))
          LOG_INFO("Level saved successfully.");
        else
          LOG_ERROR(L"Level save failed.");
      }

      string loadLabel = "Load (" + Helper::To_String(std::filesystem::path(PATH.GetLevelDataPath(displayIndex)).filename().wstring()) + ")";
      if (ImGui::MenuItem(loadLabel.c_str(), nullptr, false, isStop))
      {
        if (SUCCEEDED(m_Game->DeSerializeLevel(PATH.GetLevelDataPath(displayIndex))))
          LOG_INFO("Level loaded successfully.");
        else
          LOG_ERROR(L"Level load failed.");
      }

      if (!isStop)
      {
        ImGui::Separator();
        ImGui::TextDisabled("(Stop editor to Save/Load)");
      }

      ImGui::EndMenu();
    }

    // ── Level Select 메뉴 ─────────────────────────────────────────────────
    Bool isStop = (EDITOR->Get_State() == EDITOR_STATE::STOP);
    if (ImGui::BeginMenu("Level Select", isStop))
    {
        Bool isAutoLoad = EDITOR->Is_AutoLoadEnabled();
        if (ImGui::Checkbox("Auto-Load Data", &isAutoLoad))
            EDITOR->Set_AutoLoadEnabled(isAutoLoad);
        
        ImGui::Separator();

        uint32 levelCount = ClientSettingManager::GetInstance()->Get_LevelCount();
        for (uint32 i = 1; i < levelCount; ++i)
        {
            string levelName = "Level " + std::to_string(i);
            if (i == 1) levelName += " (Loading)";
            else if (i == 2) levelName += " (Title)";
            else if (i == 3) levelName += " (Gameplay)";

            if (ImGui::MenuItem(levelName.c_str()))
            {
                EDITOR->Request_Reset(i);
            }
        }
        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

}

Shared<MenuBar> MenuBar::Create() {
  auto menuBar = make_shared<MenuBar>();

  if (FAILED(menuBar->Initialize())) {
    MSG_BOX("Failed To Create MenuBar");
    return nullptr;
  }

  return menuBar;
}