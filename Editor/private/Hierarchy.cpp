#include "pch.h"
#include "Hierarchy.h"
#include "EditorManager.h"
#include "Game.h"
#include "GameObject.h"



using namespace Engine;

Hierarchy::Hierarchy() : EditorObject{} {}
Hierarchy::~Hierarchy() {}

HRESULT Hierarchy::Initialize() { return EditorObject::Initialize(); }

void Hierarchy::Update() {
  if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
    Delete_Selected();
  }

  EditorObject::Update();
}

void Hierarchy::Render() {
  ImGui::Begin("Hierarchy");

  auto& allObjects = GAME_INSTANCE->Get_GameObjects();

  for (auto &[id, pObj] : allObjects) {
    if (!pObj->Has_Parent()) {
      Render_Node(pObj);
    }
  }

  ImGui::End();
}

void Hierarchy::Render_Node(const Shared<GameObject> &pObj) {
  const auto& children = pObj->Get_Children();

  ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

  if (children.empty())
    flags |= ImGuiTreeNodeFlags_Leaf;

  // 현재 선택된 오브젝트면 하이라이트
  Shared<GameObject> selected = EDITOR->Get_SelectedObject();
  if (selected && selected->Get_ObjectID() == pObj->Get_ObjectID())
    flags |= ImGuiTreeNodeFlags_Selected;

  // wstring → UTF-8 string 변환 (ImGui는 UTF-8 기준)
  const wstring &wName = pObj->Get_Name();
  int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wName.c_str(), -1, nullptr, 0,
                                    nullptr, nullptr);
  string name(utf8Len, '\0');
  WideCharToMultiByte(CP_UTF8, 0, wName.c_str(), -1, name.data(), utf8Len,
                      nullptr, nullptr);

  // void* 기반 고유 ID로 TreeNode 중복 방지
  bool bOpened = ImGui::TreeNodeEx(
      reinterpret_cast<void *>(static_cast<intptr_t>(pObj->Get_ObjectID())),
      flags, "%s", name.c_str());

  // 클릭 감지 → Inspector 연동
  if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
    EDITOR->Set_SelectedObject(pObj);
  }

  if (bOpened) {
    for (auto &child : children) {
      Render_Node(child);
    }
    ImGui::TreePop();
  }
}

void Hierarchy::Delete_Selected() {
  Shared<GameObject> selected = EDITOR->Get_SelectedObject();
  if (!selected)
    return;

  // Object::Destroy() → ObjectManager가 Update 종료 후 자동 정리
  Object::Destroy(selected);
  EDITOR->Clear_SelectedObject();
}

Shared<Hierarchy> Hierarchy::Create() {
  auto hierarchy = make_shared<Hierarchy>();

  if (FAILED(hierarchy->Initialize())) {
    MSG_BOX("Failed to Create Hierarchy");
    return nullptr;
  }

  return hierarchy;
}
