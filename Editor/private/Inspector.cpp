#include "pch.h"
#include "Inspector.h"
#include "PathManager.h"

Inspector::Inspector() {}
Inspector::~Inspector() {}

HRESULT Inspector::Initialize()
{
	m_layerJsonPath = PATH.GetLayerSettingsPath();
    m_tagJsonPath = PATH.GetTagSettingsPath();

	auto layerRegistry = GAME->Get_LayerRegister();
	auto tagRegistry = GAME->Get_TagRegister();

	layerRegistry->LoadFromFile(m_layerJsonPath);
	tagRegistry->LoadFromFile(m_tagJsonPath);

	return EditorObject::Initialize();
}

void Inspector::Update()
{

}

void Inspector::Render()
{
	ImGui::Begin("Inspector");
	{
		Inspector::LayerTagGUI();
		ImGui::Separator();
		ImGui::TextDisabled("Select a GameObject to view properties");
	}
	ImGui::End();
}

void Inspector::LayerTagGUI()
{
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 4.0f));

    if (ImGui::BeginTable("InspectorHeader", 2, ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("Layers", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Tags", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableNextRow();

        // ================= [ 왼쪽: Layers ] =================
        ImGui::TableSetColumnIndex(0);
        if (ImGui::CollapsingHeader("Layers", ImGuiTreeNodeFlags_OpenOnArrow)) {
            ImGui::BeginChild("LayerScroll", ImVec2(0, 200), true);

            auto layerRegistry = GAME->Get_LayerRegister();

            // 맵을 도는게 아니라 0~31 인덱스로 직접 접근합니다.
            for (int i = 0; i < 32; ++i) {
                Engine::LAYER currentLayer;
                if (i == 0) currentLayer = Engine::LAYER::LAYER0;
                else currentLayer = static_cast<Engine::LAYER>(1 << (i - 1));

                // 레지스트리에서 이름 가져오기 (없으면 빈 문자열)
                std::wstring wName = layerRegistry->Get_LayerName(currentLayer);

                Char buffer[256] = {};
                wcstombs_s(nullptr, buffer, wName.c_str(), sizeof(buffer));

                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 35.f);
                std::string labelId = "##layer_" + std::to_string(i);

                if (ImGui::InputText(labelId.c_str(), buffer, sizeof(buffer))) {
                    tChar wBuffer[256] = {};
                    mbstowcs_s(nullptr, wBuffer, buffer, sizeof(wBuffer));
                    layerRegistry->Set_LayerName(currentLayer, wBuffer);
                }

                ImGui::SameLine();
                ImGui::TextDisabled("%2d", i);
            }
            ImGui::EndChild();
        }

        // ================= [ 오른쪽: Tags ] =================
        ImGui::TableSetColumnIndex(1);
        if (ImGui::CollapsingHeader("Tags", ImGuiTreeNodeFlags_OpenOnArrow)) {
            ImGui::BeginChild("TagScroll", ImVec2(0, 200), true);

            auto tagRegistry = GAME->Get_TagRegister();

            // 태그도 동일하게 0~31 혹은 정해진 개수만큼 반복
            for (int i = 0; i < 32; ++i) {
                Engine::TAG currentTag = static_cast<Engine::TAG>(i);
                if (i == 0) currentTag = Engine::TAG::TAG_0;
                else currentTag = static_cast<Engine::TAG>(1 << (i - 1));

                std::wstring wName = tagRegistry->Get_TagName(currentTag);

                Char buffer[256] = {};
                wcstombs_s(nullptr, buffer, wName.c_str(), sizeof(buffer));

                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 35.f);
                std::string labelId = "##tag_" + std::to_string(i);

                if (ImGui::InputText(labelId.c_str(), buffer, sizeof(buffer))) {
                    tChar wBuffer[256] = {};
                    mbstowcs_s(nullptr, wBuffer, buffer, sizeof(wBuffer));
                    tagRegistry->Set_TagName(currentTag, wBuffer);
                }

                ImGui::SameLine();
                ImGui::TextDisabled("%2d", i);
            }
            ImGui::EndChild();
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
}

Shared<Inspector> Inspector::Create()
{
	auto inspector = make_shared<Inspector>();

	if (FAILED(inspector->Initialize()))
	{
		MSG_BOX("Failed To Create Inspector");
	}

	return inspector;
}
