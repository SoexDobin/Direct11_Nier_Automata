#include "pch.h"
#include "MenuBar.h"
#include "PathManager.h"
#include "EditorManager.h"
#include "ModelViewer.h"
#include "AnimationPresetEditor.h"
#include "ClientSettingManager.h"
#include "InputDevice.h"    
#include "CollisionManager.h"
#include "Navigation.h"
#include "NavHelper.h"


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
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "Ctrl+Z", false, EDITOR->Can_Undo()))
				EDITOR->Queue_Undo();
			if (ImGui::MenuItem("Redo", "Ctrl+Y", false, EDITOR->Can_Redo()))
				EDITOR->Queue_Redo();
			ImGui::EndMenu();
		}

        Render_Debug(); // Debug Rays
		Render_Prefab();

        if (ImGui::BeginMenu("WindowSetting")) {
          ImGui::Separator();

          Bool showModelViewer = EDITOR->Get_ModelViewer()->Is_Enabled();
          if (ImGui::MenuItem("Model Viewer", nullptr, showModelViewer))
              EDITOR->Get_ModelViewer()->Set_Enable(!showModelViewer);

		  Bool showAnimationPreset = EDITOR->Get_AnimationPresetEditor()->Is_Enabled();
		  if (ImGui::MenuItem("Animation Preset", nullptr, showAnimationPreset))
			  EDITOR->Get_AnimationPresetEditor()->Set_Enable(!showAnimationPreset);

          Bool showNavHelper = EDITOR->Get_NavHelper()->Is_Enabled();
          if (ImGui::MenuItem("NavMesh Builder", nullptr, showNavHelper))
              EDITOR->Get_NavHelper()->Set_Enable(!showNavHelper);

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
		  {
			EDITOR->Clear_History();
			EDITOR->Clear_SelectedObject();
			LOG_INFO("Level loaded successfully.");
		  }
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
                if (layerCount > 0 && ImGui::BeginTable("CollisionMatrixTable", static_cast<int32>(layerCount + 1), ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit))
                {
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextDisabled("/");
                    // 상단 가로축 헤더
                    for (size_t i = 0; i < layerCount; ++i) {
                        ImGui::TableSetColumnIndex(static_cast<int32>(i + 1));
                        ImGui::Text(activeLayers[layerCount - 1 - i].second.substr(0, 3).c_str());
                    }
                    // 행 그리기 (계단식)
                    for (size_t row = 0; row < layerCount; ++row)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text(activeLayers[row].second.c_str()); // 좌측 세로축 헤더
                        uint32 rowBit = activeLayers[row].first;
                        for (size_t col = 0; col < layerCount - row; ++col)
                        {
                            ImGui::TableSetColumnIndex(static_cast<int32>(col + 1));
                            uint32 colBit = activeLayers[layerCount - 1 - col].first;

                            bool isColliding = (layerReg->Get_GlobalMask(rowBit) & colBit) != 0;
                            ImGui::PushID(static_cast<int32>(row * 100 + col));
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

void MenuBar::Render_Prefab()
{
	const Bool isStop = EDITOR->Get_State() == EDITOR_STATE::STOP;
	if (!ImGui::BeginMenu("Prefab", isStop))
		return;

	ImGui::SetNextItemWidth(180.f);
	ImGui::InputTextWithHint("##PrefabName", "Prefab name", m_PrefabName,
		static_cast<size_t>(MAX_PATH));
	const Shared<GameObject> selectedRoot = EDITOR->Get_SelectedObject();
	const Bool canSave = selectedRoot && selectedRoot->Get_StableChildKey().empty() &&
		m_PrefabName[0] != '\0';
	if (ImGui::MenuItem("Save Selected", nullptr, false, canSave))
	{
		const filesystem::path inputPath(Helper::To_wString(m_PrefabName));
		filesystem::path fileName = inputPath.filename();
		if (fileName.empty() || fileName != inputPath)
		{
			LOG_ERROR("Prefab name must not contain a directory path.");
		}
		else
		{
			fileName.replace_extension(L".json");
			const filesystem::path filePath =
				filesystem::path(PATH.GetPrefabSettingsDir()) / fileName;
			PrefabGuid prefabGuid{};
			Bool registeredNewGuid = false;
			if (filesystem::exists(filePath))
			{
				if (FAILED(m_Game->Register_PrefabDocument(filePath.wstring(), prefabGuid)))
					LOG_ERROR(L"Existing Prefab document is invalid: {}", filePath.wstring());
			}
			else
			{
				prefabGuid = Create_PrefabGuid();
				registeredNewGuid = prefabGuid.Is_Valid() &&
					SUCCEEDED(m_Game->Register_Prefab(prefabGuid, filePath.wstring()));
			}

			if (prefabGuid.Is_Valid() &&
				SUCCEEDED(m_Game->SerializePrefabDocument(prefabGuid, selectedRoot)))
				LOG_INFO(L"Prefab saved: {}", filePath.wstring());
			else
			{
				if (registeredNewGuid)
					m_Game->Unregister_Prefab(prefabGuid);
				LOG_ERROR(L"Prefab save failed: {}", filePath.wstring());
			}
		}
	}
	if (!selectedRoot)
		ImGui::TextDisabled("Select a hierarchy root to save.");
	else if (!selectedRoot->Get_StableChildKey().empty())
		ImGui::TextDisabled("Code-defined children cannot be Prefab roots.");

	ImGui::Separator();
	if (ImGui::BeginMenu("Instantiate"))
	{
		const vector<pair<PrefabGuid, wstring>> prefabDocuments =
			m_Game->Get_PrefabDocuments();
		if (prefabDocuments.empty())
			ImGui::TextDisabled("No Prefab documents.");
		for (const auto& [prefabGuid, prefabPathString] : prefabDocuments)
		{
			const filesystem::path prefabPath(prefabPathString);
			const string label = Helper::To_String(prefabPath.stem().wstring());
			if (!ImGui::MenuItem(label.c_str()))
				continue;

			Shared<GameObject> instantiatedRoot;
			if (SUCCEEDED(m_Game->DeSerializePrefabDocument(prefabGuid, instantiatedRoot)))
			{
				EDITOR->Set_SelectedObject(instantiatedRoot);
				LOG_INFO(L"Prefab instantiated: {}", prefabPath.wstring());
			}
			else
				LOG_ERROR(L"Prefab instantiate failed: {}", prefabPath.wstring());
		}
		ImGui::EndMenu();
	}

	ImGui::EndMenu();
}

void MenuBar::Update_HotKey()
{
	const ImGuiIO& io = ImGui::GetIO();
	if (!io.WantTextInput && io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z, false))
		EDITOR->Queue_Undo();
	if (!io.WantTextInput && io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y, false))
		EDITOR->Queue_Redo();
	if (!io.WantTextInput && io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_D, false))
	{
		const Shared<GameObject> selected = EDITOR->Get_SelectedObject();
		if (selected && EDITOR->Can_EditHierarchy(selected))
		{
			uint32 levelIndex = GAME_INSTANCE->Get_CurrentLevelIndex();
			if (!GAME_INSTANCE->Contains(levelIndex, selected->Get_ObjectGuid()))
				levelIndex = 0;
			EDITOR->Queue_Duplicate(selected->Get_ObjectGuid(), levelIndex);
		}
	}

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
