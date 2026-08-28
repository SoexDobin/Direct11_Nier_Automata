#include "pch.h"
#include "Hierarchy.h"
#include "EditorManager.h"
#include "Game.h"
#include "GameObject.h"
#include "ID_Helper.h"



using namespace Engine;

Hierarchy::Hierarchy() : EditorObject{} {}
Hierarchy::~Hierarchy() {}

HRESULT Hierarchy::Initialize() { return EditorObject::Initialize(); }

void Hierarchy::Update(Bool isResize) {
  if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
    Delete_Selected();
  }

  EditorObject::Update(isResize);
}

void Hierarchy::Render(Bool isResize) {
    ImGui::Begin("Hierarchy");

    // 1. Render Static Level (0)
    auto& staticObjects = GAME_INSTANCE->Get_GameObjects(0);
    for (auto &[id, pObj] : staticObjects) {
        if (!pObj->Has_Parent()) {
            Render_Node(pObj);
        }
    }

    // 2. Render Current Level
    uint32 currentLevel = GAME_INSTANCE->Get_CurrentLevelIndex();
    if (currentLevel != 0) {
        auto& currentLevelObjects = GAME_INSTANCE->Get_GameObjects(currentLevel);
        for (auto &[id, pObj] : currentLevelObjects) {
            if (!pObj->Has_Parent()) {
                Render_Node(pObj);
            }
        }
    }

    // ── 드래그-드롭 수신 영역 ────────────────────────────────────────────
    // Hierarchy 창 전체 영역에 대해 드롭 수신 시도
    ImGui::Dummy(ImGui::GetContentRegionAvail());
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(Drag_PayLoadKey.c_str()))
        {
            const wchar_t *rawTag = static_cast<const wchar_t *>(payload->Data);
            wstring prototypeTag(rawTag);

            Shared<GameObject> cloned = GAME_INSTANCE->Instantiate<GameObject>(prototypeTag, GAME_INSTANCE->Get_CurrentLevelIndex());
            if (cloned)
            {
                auto allObjs = GAME_INSTANCE->Get_GameObjects(GAME_INSTANCE->Get_CurrentLevelIndex());
                int suffix = 0;
                wstring baseName = cloned->Get_Name();
                wstring uniqueName = baseName;
                while (true) {
                    uniqueName = (suffix == 0) ? baseName : baseName + L"_" + std::to_wstring(suffix);
                    bool overlap = false;
                    for (auto& [id, obj] : allObjs) {
                        if (obj != cloned && obj->Get_Name() == uniqueName) { overlap = true; break; }
                    }
                    if (!overlap) break;
                    suffix++;
                }
                cloned->Set_Name(uniqueName);
                cloned->Set_ObjectID(Helper::Create_FixedObjectID(prototypeTag, uniqueName));

				LOG_INFO(L"[Hierarchy] Spawned from prefab: {}", prototypeTag);
            }
            else
				LOG_WARN(L"[Hierarchy] Failed to spawn: {}", prototypeTag);
        }
        
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(ObjectMove_PayLoadKey.c_str()))
        {
            uint32 draggedID = *static_cast<uint32*>(payload->Data);
            Shared<GameObject> draggedObj = GAME_INSTANCE->Find_ByInstanceID(GAME_INSTANCE->Get_CurrentLevelIndex(), draggedID);
            if (draggedObj)
            {
                if (!draggedObj->Get_StableChildKey().empty())
                {
                    LOG_WARN(L"[Hierarchy] Code-defined child cannot be moved to Root: {}", draggedObj->Get_Name());
                }
                else if (SUCCEEDED(draggedObj->Remove_Parent()))
                {
                    LOG_INFO(L"[Hierarchy] Moved {} to Root", draggedObj->Get_Name());
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::End();
}

void Hierarchy::Render_Node(const Shared<GameObject> &pObj) {
    const auto& children = pObj->Get_Children();
    Bool hasCodeDefinedStructure = !pObj->Get_StableChildKey().empty();
    for (const auto& child : children)
    {
        if (child && !child->Get_StableChildKey().empty())
        {
            hasCodeDefinedStructure = true;
            break;
        }
    }

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (children.empty())
      flags |= ImGuiTreeNodeFlags_Leaf;

    // 현재 선택된 오브젝트면 하이라이트
    Shared<GameObject> selected = EDITOR->Get_SelectedObject();
    if (selected && selected->Get_ObjectID() == pObj->Get_ObjectID())
      flags |= ImGuiTreeNodeFlags_Selected;

    // wstring → UTF-8 string 변환 (ImGui는 UTF-8 기준)
    const wstring &wName = Clean_RTTR_Name(pObj->Get_Name());
    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wName.c_str(), -1, nullptr, 0,
                                      nullptr, nullptr);
    string name(utf8Len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wName.c_str(), -1, name.data(), utf8Len,
                        nullptr, nullptr);

    bool bOpened = ImGui::TreeNodeEx(
        reinterpret_cast<void *>(static_cast<intptr_t>(pObj->Get_InstanceID())), // ObjectID 대신 InstanceID 사용 권장 (중복 클릭 방지)
        flags, "%s", name.c_str());

    // ── 노드 단위 드롭 수신 (객체 이동) ───────────────────────────
    if (pObj->Get_StableChildKey().empty() && ImGui::BeginDragDropSource())
    {
        uint32 instanceID = pObj->Get_InstanceID();
        ImGui::SetDragDropPayload(ObjectMove_PayLoadKey.c_str(), &instanceID, sizeof(uint32));
        ImGui::Text("Moving: %s", name.c_str());
        ImGui::EndDragDropSource();
    }

    // ── 노드 단위 드롭 수신 (자식으로 생성) ───────────────────────────
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("PREFAB_DRAG"))
        {
            const wchar_t *rawTag = static_cast<const wchar_t *>(payload->Data);
            wstring prototypeTag(rawTag);

            if (hasCodeDefinedStructure)
            {
                LOG_WARN(L"[Hierarchy] Cannot add Editor child to code-defined hierarchy: {}", pObj->Get_Name());
            }
            else
            {
                // [핵심] 현재 노드(pObj)를 부모로 하여 생성
                Shared<GameObject> cloned = GAME_INSTANCE->Instantiate<GameObject>(prototypeTag, UINT_MAX);
                if (cloned)
                {
                    auto allObjs = GAME_INSTANCE->Get_GameObjects(GAME_INSTANCE->Get_CurrentLevelIndex());
                    int suffix = 0;
                    wstring baseName = cloned->Get_Name();
                    wstring uniqueName = baseName;
                    while (true) {
                        uniqueName = (suffix == 0) ? baseName : baseName + L"_" + std::to_wstring(suffix);
                        bool overlap = false;
                        for (auto& [id, obj] : allObjs) {
                            if (obj != cloned && obj->Get_Name() == uniqueName) { overlap = true; break; }
                        }
                        if (!overlap) break;
                        suffix++;
                    }
                    cloned->Set_Name(uniqueName);
                    cloned->Set_ObjectID(Helper::Create_FixedObjectID(prototypeTag, uniqueName));

                    cloned->Set_Parent(pObj);
                    LOG_INFO(L"[Hierarchy] Spawned {} as child of {}", prototypeTag, pObj->Get_Name());
                }
            }
        }

        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(ObjectMove_PayLoadKey.c_str()))
        {
            uint32 draggedID = *static_cast<uint32*>(payload->Data);
            Shared<GameObject> draggedObj = GAME_INSTANCE->Find_ByInstanceID(GAME_INSTANCE->Get_CurrentLevelIndex(), draggedID);

            // 순환 참조 방지 로직 (자신이 부모가 되거나, 조상을 자신의 자식으로 넣는 경우 방지)
            if (draggedObj && draggedObj != pObj)
            {
                if (!draggedObj->Get_StableChildKey().empty())
                {
                    LOG_WARN(L"[Hierarchy] Code-defined child cannot be moved: {}", draggedObj->Get_Name());
                }
                else if (hasCodeDefinedStructure)
                {
                    LOG_WARN(L"[Hierarchy] Cannot modify code-defined hierarchy: {}", pObj->Get_Name());
                }
                else
                {
                    // 단순 체크: pObj가 draggedObj의 자손인지 확인
                    bool isDescendant = false;
                    Shared<GameObject> current = pObj;
                    while (current) {
                        if (current == draggedObj) {
                            isDescendant = true;
                            break;
                        }
                        current = current->Get_Parent();
                    }

                    if (!isDescendant)
                    {
                        draggedObj->Set_Parent(pObj);
                        LOG_INFO(L"[Hierarchy] Moved {} to child of {}", draggedObj->Get_Name(), pObj->Get_Name());
                    }
                    else
                    {
                        LOG_WARN(L"[Hierarchy] Cannot move ancestor as child of descendant!");
                    }
                }
            }
        }

        ImGui::EndDragDropTarget();
    }

    // 클릭 감지 → Inspector 연동 (드래그 중에는 선택 변경 방지)
    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && !ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
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

    if (!selected->Get_StableChildKey().empty())
    {
      LOG_WARN(L"[Hierarchy] Code-defined child cannot be deleted: {}", selected->Get_Name());
      return;
    }

    Object::Destroy(selected);
    EDITOR->Clear_SelectedObject();

    GAME_INSTANCE->Clearing_ObjectManager(GAME_INSTANCE->Get_CurrentLevelIndex());
}

Shared<Hierarchy> Hierarchy::Create() {
  auto hierarchy = make_shared<Hierarchy>();

  if (FAILED(hierarchy->Initialize())) {
    MSG_BOX("Failed to Create Hierarchy");
    return nullptr;
  }

  return hierarchy;
}
