#include "PrefabTab.h"
#include "EditorManager.h"
#include "Inspector.h"
#include "MetadataManager.h"
#include "pch.h"
#include <imgui.h>


PrefabTab::PrefabTab() {}
PrefabTab::~PrefabTab() {}

HRESULT PrefabTab::Initialize() { return EditorObject::Initialize(); }

void PrefabTab::Render() {
  ImGui::Begin("Prefab");

  ImGui::Text("Prefab Classes");
  ImGui::Separator();

  RenderCategoryFolder("GameObject", "gameobject");
  RenderCategoryFolder("Level", "level");
  RenderCategoryFolder("Component", "component");

  ImGui::End();
}

void PrefabTab::RenderCategoryFolder(const char *label, const char *category) {
  auto metadata = EDITOR->Get_MetadataManager();
  if (!metadata)
    return;

  auto items = metadata->GetByCategory(category);
  if (items.empty())
    return;

  /* 폴더 아이콘 스타일 TreeNode */
  ImGuiTreeNodeFlags folderFlags = ImGuiTreeNodeFlags_OpenOnArrow |
                                   ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                   ImGuiTreeNodeFlags_DefaultOpen;

  /* 폴더 아이콘 색상 */
  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.85f, 0.3f, 1.0f));
  bool folderOpen = ImGui::TreeNodeEx(label, folderFlags);
  ImGui::PopStyleColor();

  if (!folderOpen)
    return;

  for (auto &[className, classInfo] : items) {
    ImGuiTreeNodeFlags leafFlags = ImGuiTreeNodeFlags_Leaf |
                                   ImGuiTreeNodeFlags_NoTreePushOnOpen |
                                   ImGuiTreeNodeFlags_SpanAvailWidth;

    if (m_SelectedClass == className)
      leafFlags |= ImGuiTreeNodeFlags_Selected;

    /* 파일 아이콘 색상 (연한 파랑) */
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.8f, 1.0f, 1.0f));
    ImGui::TreeNodeEx(className.c_str(), leafFlags);
    ImGui::PopStyleColor();

    if (ImGui::IsItemClicked()) {
      m_SelectedClass = className;

      if (auto inspector = m_Inspector.lock())
        inspector->Set_SelectedClass(className);
    }

    /* 호버 시 툴팁 */
    if (ImGui::IsItemHovered()) {
      ImGui::BeginTooltip();
      ImGui::Text("Class: %s", className.c_str());
      if (!classInfo->baseClass.empty())
        ImGui::Text("Base: %s", classInfo->baseClass.c_str());
      ImGui::Text("Category: %s", category);
      ImGui::Text("Members: %d own, %d inherited",
                  (int)classInfo->members.size(),
                  (int)classInfo->inheritedMembers.size());
      ImGui::EndTooltip();
    }
  }

  ImGui::TreePop();
}

Shared<PrefabTab> PrefabTab::Create() {
  auto instance = make_shared<PrefabTab>();

  if (FAILED(instance->Initialize())) {
    MSG_BOX("Failed To Create PrefabTab");
    return nullptr;
  }

  return instance;
}
