#include "pch.h"
#include "MenuBar.h"
#include "PathManager.h"
#include "EditorManager.h"
#include "ModelViewer.h"
#include "ClientSettingManager.h"
#include "InputDevice.h"    
#include "CollisionManager.h"
#include "Navigation.h"


MenuBar::MenuBar() {}

HRESULT MenuBar::Initialize() {
    m_Enable = false;
    m_Game = GAME_INSTANCE;
    return EditorObject::Initialize();
}
void MenuBar::Update(Bool isResize)
{
    Update_HotKey();
	EditorObject::Update(isResize);
}
void MenuBar::Render(Bool isResize) {
  EditorObject::Render(isResize);
	if (ImGui::BeginMainMenuBar()) {

        Render_Debug(); // Debug Rays

        if (ImGui::BeginMenu("WindowSetting")) {
          ImGui::Separator();

          Bool showModelViewer = EDITOR->Get_ModelViewer()->Is_Enabled();
          if (ImGui::MenuItem("Model Viewer", nullptr, showModelViewer))
              EDITOR->Get_ModelViewer()->Set_Enable(!showModelViewer);

          ImGui::MenuItem("Project Settings", nullptr, &m_ShowProjectSettings);

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
          if (SUCCEEDED(m_Game->SerializeLevel(GAME_INSTANCE->Get_CurrentLevelIndex(), PATH.GetLevelDataPath(displayIndex))))
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
    Float rightAlignPos = ImGui::GetWindowWidth() - 30.0f;
    ImGui::SetCursorPosX(rightAlignPos);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));

    // X 버튼 클릭 시 프로세스 종료 메시지 호출
    if (ImGui::MenuItem("X")) {
        
        if (m_Game && m_Game->Get_Device() && m_Game->Get_SwapChain()) {
            m_Game->Get_SwapChain()->SetFullscreenState(FALSE, nullptr);
        }

        PostQuitMessage(0); // Win32 응용 프로그램 안전 종료
    }

    ImGui::PopStyleColor();

    ImGui::EndMainMenuBar();
	}

    //??
    if (m_ShowProjectSettings)
    {
        if (ImGui::Begin("Project Settings", &m_ShowProjectSettings))
        {
            auto layerReg = GAME_INSTANCE->Get_LayerRegister();
            auto tagReg = GAME_INSTANCE->Get_TagRegister();
            // 1. 저장 버튼 (수정 내용을 곧바로 Json에 쓰고 터미널에 띄움)
            if (ImGui::Button("Save Configuration")) {
                layerReg->SaveToFile(PATH.GetLayerSettingsPath());
                if (tagReg) tagReg->SaveToFile(PATH.GetTagSettingsPath());

                LOG_INFO("Project Settings (Layers & Tags & Global Matrix) Saved to JSON.");
            }
            ImGui::SameLine(); ImGui::TextDisabled("<- Manually Save to disk");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            if (ImGui::CollapsingHeader("Global Collision Matrix", ImGuiTreeNodeFlags_DefaultOpen))
            {
                // 32개의 레이어를 모두 표시 (이름 지정 여부 상관없음)
                std::vector<std::pair<uint32, string>> activeLayers;
                for (int i = 0; i < 32; ++i) {
                    uint32 bit = (1 << i);
                    string name = Helper::To_String(layerReg->Get_LayerName(bit));
                    activeLayers.push_back({ bit, name });
                }
                size_t layerCount = activeLayers.size();

                // 표(Table) UI 생성
                if (layerCount > 0 && ImGui::BeginTable("CollisionMatrixTable", layerCount + 1, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit))
                {
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextDisabled("/");
                    // 상단 가로축 헤더
                    for (int i = 0; i < layerCount; ++i) {
                        ImGui::TableSetColumnIndex(i + 1);
                        ImGui::Text(activeLayers[layerCount - 1 - i].second.substr(0, 3).c_str());
                    }
                    // 행 그리기 (계단식)
                    for (int row = 0; row < layerCount; ++row)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text(activeLayers[row].second.c_str()); // 좌측 세로축 헤더
                        uint32 rowBit = activeLayers[row].first;
                        for (int col = 0; col < layerCount - row; ++col)
                        {
                            ImGui::TableSetColumnIndex(col + 1);
                            uint32 colBit = activeLayers[layerCount - 1 - col].first;

                            bool isColliding = (layerReg->Get_GlobalMask(rowBit) & colBit) != 0;
                            ImGui::PushID(row * 100 + col);
                            if (ImGui::Checkbox("##col_mat", &isColliding))
                            {
                                // 상호 양방향 충돌 스위치 동시 처리
                                layerReg->Toggle_GlobalMask_Symmetric(rowBit, colBit);
                            }
                            if (ImGui::IsItemHovered())
                            {
                                string rowName = activeLayers[row].second;
                                string colName = activeLayers[layerCount - 1 - col].second;
                                ImGui::SetTooltip("%s vs %s", rowName.c_str(), colName.c_str());
                            }
                            ImGui::PopID();
                        }
                    }
                    ImGui::EndTable();
                }
            }
        }
        ImGui::End();
    }
}

void MenuBar::Update_HotKey()
{
    Bool currF1 = (GAME_INSTANCE->Get_DIKeyState(DIK_F1) & 0x80) != 0;
    Bool currF2 = (GAME_INSTANCE->Get_DIKeyState(DIK_F2) & 0x80) != 0;

    if (currF1 && !m_PrevF1)
    {
        auto result = GAME_INSTANCE->Toggle_RenderDebug();
    }

    if (currF2 && !m_PrevF2)
    {
        Navigation::Toggle_DebugRender();
    }

    m_PrevF1 = currF1;
    m_PrevF2 = currF2;
}

void MenuBar::Render_Debug()
{
    if (ImGui::BeginMenu("Debug"))
    {
        Bool colDebug = GAME_INSTANCE->Toggle_RenderDebug();

        if (ImGui::MenuItem("Toggle DebugRender Collider", "F1", &colDebug))
			;

        Bool navDebug = Navigation::Get_DebugRender();
        if (ImGui::MenuItem("Toggle DebugRender Navigation", "F2", &navDebug))
            ;
        

        ImGui::EndMenu();
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
