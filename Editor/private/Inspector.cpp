#include "Inspector.h"
#include "EditorManager.h"
#include "pch.h"


#include "Component.h"
#include "GameObject.h"
#include "LayerRegistry.h"
#include "PathManager.h"
#include "TagRegistry.h"
#include "Transform.h"


using namespace Engine;

Inspector::Inspector() {}
Inspector::~Inspector() {}

HRESULT Inspector::Initialize() {
  auto layerRegistry = GAME_INSTANCE->Get_LayerRegister();
  auto tagRegistry = GAME_INSTANCE->Get_TagRegister();

  layerRegistry->LoadFromFile(PATH.GetLayerSettingsPath());
  tagRegistry->LoadFromFile(PATH.GetTagSettingsPath());

  return EditorObject::Initialize();
}

void Inspector::Render() {
  ImGui::Begin("Inspector");

  LayerTagGUI();
  ImGui::Separator();

  Shared<GameObject> selected = EDITOR->Get_SelectedObject();
  if (!selected) {
    ImGui::TextDisabled("No object selected.");
    ImGui::End();
    return;
  }

  GameObjectGUI(selected);

  ImGui::End();
}

// ====================================================
// wstring -> UTF-8 string 변환 헬퍼
// ====================================================
static string WStr_To_UTF8(const wstring &wStr) {
  if (wStr.empty())
    return {};
  int len = WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), -1, nullptr, 0,
                                nullptr, nullptr);
  string result(len, '\0');
  WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), -1, result.data(), len, nullptr,
                      nullptr);
  return result;
}

// ====================================================
// GameObjectGUI: 선택 오브젝트의 이름・Transform・컴포넌트 표시
// ====================================================
void Inspector::GameObjectGUI(const Shared<GameObject> &pObj) {
  // ── 오브젝트 이름
  string name = WStr_To_UTF8(pObj->Get_Name());
  ImGui::Text("Name : %s", name.c_str());
  ImGui::Text("ID   : %u", pObj->Get_ObjectID());
  ImGui::Separator();

  // ── Transform 섹션
  Shared<Transform> pTransform = pObj->Get_Transform();
  if (pTransform) {
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
      // Position
      Vector3 pos = pTransform->Get_LocalPosition();
      float fPos[3] = {pos.x, pos.y, pos.z};
      if (ImGui::DragFloat3("Position", fPos, 0.1f)) {
        pTransform->Set_LocalPosition(fPos[0], fPos[1], fPos[2]);
      }

      // Rotation (Euler)
      Vector3 rot = pTransform->Get_LocalEulerAngles();
      float fRot[3] = {rot.x, rot.y, rot.z};
      if (ImGui::DragFloat3("Rotation", fRot, 0.5f)) {
        pTransform->Set_LocalRotation(fRot[0], fRot[1], fRot[2]);
      }

      // Scale
      Vector3 scl = pTransform->Get_LocalScale();
      float fScl[3] = {scl.x, scl.y, scl.z};
      if (ImGui::DragFloat3("Scale", fScl, 0.01f)) {
        pTransform->Set_LocalScale(fScl[0], fScl[1], fScl[2]);
      }
    }
  }

  ImGui::Separator();

  // ── Components 섹션 (Transform 제외)
  const auto &components = pObj->Get_Components();
  if (!components.empty()) {
    ImGui::Text("Components (%zu)", components.size());
    for (auto &[id, pComp] : components) {
      // Transform은 이미 위에서 표시했으므로 중복 제외
      if (pComp->Get_ComponentType() == COMPONENT_TYPE::TRANSFORM)
        continue;

      string typeName = rttr::type::get(*pComp).get_name().to_string();
      ComponentGUI(typeName, pComp);
    }
  }

  ImGui::Separator();

  // ── Scripts 섹션
  const auto &scripts = pObj->Get_Scripts();
  if (!scripts.empty()) {
    ImGui::Text("Scripts (%zu)", scripts.size());
    for (auto &[id, pScript] : scripts) {
      string typeName = rttr::type::get(*pScript).get_name().to_string();
      ComponentGUI(typeName, pScript);
    }
  }
}

// ====================================================
// ComponentGUI: 개별 컴포넌트 헤더 + RTTR 프로퍼티 표시
// ====================================================
void Inspector::ComponentGUI(const string &label,
                             const Shared<Component> &pComp) {
  // RTTR 기반으로 등록된 프로퍼티를 자동 열거
  ImGui::PushID(static_cast<int>(pComp->Get_ObjectID()));

  if (ImGui::CollapsingHeader(label.c_str())) {
    auto rttrType = rttr::type::get(*pComp);
    for (auto &prop : rttrType.get_properties()) {
      string propName = prop.get_name().to_string();
      rttr::variant value = prop.get_value(*pComp);

      // 타입별 ImGui 위젯 분기 (기본 타입 한정)
      if (value.is_type<float>()) {
        float v = value.get_value<float>();
        if (ImGui::DragFloat(propName.c_str(), &v, 0.01f)) {
          prop.set_value(*pComp, v);
        }
      } else if (value.is_type<int>() || value.is_type<int32_t>()) {
        int v = value.get_value<int>();
        if (ImGui::DragInt(propName.c_str(), &v)) {
          prop.set_value(*pComp, v);
        }
      } else if (value.is_type<bool>()) {
        bool v = value.get_value<bool>();
        if (ImGui::Checkbox(propName.c_str(), &v)) {
          prop.set_value(*pComp, v);
        }
      } else {
        // 지원하지 않는 타입은 타입 이름만 표시
        ImGui::TextDisabled("  %s : <%s>", propName.c_str(),
                            value.get_type().get_name().to_string().c_str());
      }
    }
  }

  ImGui::PopID();
}

// ====================================================
// LayerTagGUI: 기존 Layer / Tag 편집 UI (변경 없음)
// ====================================================
void Inspector::LayerTagGUI() {
  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 4.0f));

  if (ImGui::BeginTable("InspectorHeader", 2, ImGuiTableFlags_Resizable)) {
    ImGui::TableSetupColumn("Layers", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Tags", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableNextRow();

    // ── Layers
    ImGui::TableSetColumnIndex(0);
    if (ImGui::CollapsingHeader("Layers", ImGuiTreeNodeFlags_OpenOnArrow)) {
      ImGui::BeginChild("LayerScroll", ImVec2(0, 200), true);

      auto layerRegistry = GAME_INSTANCE->Get_LayerRegister();

      for (int i = 0; i < 32; ++i) {
        Engine::LAYER currentLayer;
        if (i == 0)
          currentLayer = Engine::LAYER::LAYER0;
        else
          currentLayer = static_cast<Engine::LAYER>(1 << (i - 1));

        wstring wName = layerRegistry->Get_LayerName(currentLayer);
        Char buffer[256] = {};
        wcstombs_s(nullptr, buffer, wName.c_str(), sizeof(buffer));

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 35.f);
        string labelId = "##layer_" + to_string(i);

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

    // ── Tags
    ImGui::TableSetColumnIndex(1);
    if (ImGui::CollapsingHeader("Tags", ImGuiTreeNodeFlags_OpenOnArrow)) {
      ImGui::BeginChild("TagScroll", ImVec2(0, 200), true);

      auto tagRegistry = GAME_INSTANCE->Get_TagRegister();

      for (int i = 0; i < 32; ++i) {
        Engine::TAG currentTag;
        if (i == 0)
          currentTag = Engine::TAG::TAG_0;
        else
          currentTag = static_cast<Engine::TAG>(1 << (i - 1));

        wstring wName = tagRegistry->Get_TagName(currentTag);
        Char buffer[256] = {};
        wcstombs_s(nullptr, buffer, wName.c_str(), sizeof(buffer));

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 35.f);
        string labelId = "##tag_" + to_string(i);

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

Shared<Inspector> Inspector::Create() {
  auto instance = make_shared<Inspector>();

  if (FAILED(instance->Initialize())) {
    MSG_BOX("Failed To Create Inspector");
    return nullptr;
  }

  return instance;
}
