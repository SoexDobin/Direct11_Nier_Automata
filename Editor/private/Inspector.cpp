#include "pch.h"
#include "Inspector.h"
#include "EditorManager.h"
#include "MetadataManager.h"
#include "PrefabRegistry.h"
#include "PathManager.h"


Inspector::Inspector() {}
Inspector::~Inspector()
{
}

HRESULT Inspector::Initialize()
{
    auto layerRegistry = GAME->Get_LayerRegister();
    auto tagRegistry = GAME->Get_TagRegister();

    layerRegistry->LoadFromFile(PATH.GetLayerSettingsPath());
    tagRegistry->LoadFromFile(PATH.GetTagSettingsPath());

	return EditorObject::Initialize();
}

void Inspector::Render() {
  ImGui::Begin("Inspector");
  LayerTagGUI();

  if (m_SelectedClass.empty()) {
    ImGui::TextDisabled("Select a class from PrefabTab");
    ImGui::End();
    return;
  }

  auto metadata = EDITOR->Get_MetadataManager();
  auto registry = EDITOR->Get_PrefabRegistry();
  if (!metadata || !registry) {
    ImGui::End();
    return;
  }

  const auto *classInfo = metadata->FindClass(m_SelectedClass);
  if (!classInfo) {
    ImGui::Text("Class '%s' not found", m_SelectedClass.c_str());
    ImGui::End();
    return;
  }

  /* Header */
  ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.f, 1.f), "%s",
                     m_SelectedClass.c_str());
  ImGui::SameLine();
  ImGui::TextDisabled("[%s]", classInfo->category.c_str());
  if (!classInfo->baseClass.empty()) {
    ImGui::SameLine();
    ImGui::TextDisabled("< %s", classInfo->baseClass.c_str());
  }
  ImGui::Separator();

  /* ============================================================
   * Transform (GameObject 계열만)
   * ============================================================ */
  if (classInfo->category == "gameobject" &&
      !classInfo->transformMembers.empty()) {
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Indent(10.f);
      RenderMemberList(classInfo->transformMembers, registry, "Transform");
      ImGui::Unindent(10.f);
    }
  }

  /* ============================================================
   * Inherited Members (부모 클래스)
   * ============================================================ */
  if (!classInfo->inheritedMembers.empty()) {
    string label = "Inherited (" + classInfo->baseClass + ")";
    if (ImGui::CollapsingHeader(label.c_str(),
                                ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Indent(10.f);
      RenderMemberList(classInfo->inheritedMembers, registry, "Inherited");
      ImGui::Unindent(10.f);
    }
  }

  /* ============================================================
   * Own Members
   * ============================================================ */
  if (!classInfo->members.empty()) {
    string label = m_SelectedClass + " Members";
    if (ImGui::CollapsingHeader(label.c_str(),
                                ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Indent(10.f);
      RenderMemberList(classInfo->members, registry, "Own");
      ImGui::Unindent(10.f);
    }
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

void Inspector::RenderMemberList(
    const vector<MetadataManager::MemberInfo> &members,
    const Shared<PrefabRegistry> &registry, const char *sectionId) {
  for (size_t i = 0; i < members.size(); ++i) {
    const auto &member = members[i];
    string widgetId = string(sectionId) + "_" + member.name;

    /* 현재 오버라이드 값 or 기본값 */
    string currentVal = registry->Get_Override(m_SelectedClass, member.name);
    if (currentVal.empty())
      currentVal = member.defaultValue;

    ImGui::PushID(widgetId.c_str());

    /* 타입별 위젯 */
    if (member.type == "int" || member.type == "Int" ||
        member.type == "Int32" || member.type == "UINT") {
      int val = 0;
      try {
        val = std::stoi(currentVal);
      } catch (...) { }
      if (ImGui::DragInt(member.name.c_str(), &val))
        registry->Set_Override(m_SelectedClass, member.name, member.type,
                               std::to_string(val));
    } else if (member.type == "float" || member.type == "Float") {
      float val = 0.f;
      try {
        val = std::stof(currentVal);
      } catch (...) {
      }
      if (ImGui::DragFloat(member.name.c_str(), &val, 0.01f))
        registry->Set_Override(m_SelectedClass, member.name, member.type,
                               std::to_string(val));
    } else if (member.type == "bool" || member.type == "Bool" ||
               member.type == "BOOL") {
      bool val = (currentVal == "true" || currentVal == "1");
      if (ImGui::Checkbox(member.name.c_str(), &val))
        registry->Set_Override(m_SelectedClass, member.name, member.type,
                               val ? "true" : "false");
    } else if (member.type == "Vector3") {
      float v[3] = {0.f, 0.f, 0.f};
      /* parse "Vector3::One" etc. simply as 0 for now */
      if (ImGui::DragFloat3(member.name.c_str(), v, 0.01f)) {
        string s = std::to_string(v[0]) + "," + std::to_string(v[1]) + "," +
                   std::to_string(v[2]);
        registry->Set_Override(m_SelectedClass, member.name, member.type, s);
      }
    } else if (member.type == "string" || member.type == "wstring") {
      char buf[256]{};
      strncpy_s(buf, currentVal.c_str(), sizeof(buf) - 1);
      if (ImGui::InputText(member.name.c_str(), buf, sizeof(buf)))
        registry->Set_Override(m_SelectedClass, member.name, member.type, buf);
    } else {
      /* 알 수 없는 타입 — 읽기 전용 텍스트 */
      ImGui::TextDisabled("[%s]", member.type.c_str());
      ImGui::SameLine();
      ImGui::Text("%s = %s", member.name.c_str(), currentVal.c_str());
    }

    ImGui::PopID();
  }
}

Shared<Inspector> Inspector::Create() {
  auto instance = make_shared<Inspector>();

  if (FAILED(instance->Initialize())) {
    MSG_BOX("Failed To Create Inspector");
    return nullptr;
  }

  return instance;
}
