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
// GameObjectGUI: 선택 오브젝트의 이름・Layer/Tag・Transform・컴포넌트 표시
// ====================================================
void Inspector::GameObjectGUI(const Shared<GameObject> &pObj) {
	// ── 오브젝트 이름
	string name = Helper::To_String(Clean_RTTR_Name(pObj->Get_Name()));
	ImGui::Text("Name : %s", name.c_str());
	ImGui::Text("ID   : %u", pObj->Get_ObjectID());
	ImGui::Separator();

  // ── RTTR GameObject 프로퍼티 (LayerNames, TagNames 등)
  GameObjectPropertiesGUI(pObj);
  ImGui::Separator();

  // ── Transform 섹션
  Shared<Transform> pTransform = pObj->Get_Transform();
  if (pTransform) {
    string typeName = rttr::type::get(*pTransform).get_name().to_string();
    ComponentGUI(typeName, pTransform);
  }
  ImGui::Separator();

  // ── Components 섹션 (Transform 제외)
  const auto &components = pObj->Get_Components();
  if (!components.empty()) {
    // Transform 은 위에서 이미 그렸으니 카운트 제외
    size_t count = 0;
    for (auto &[id, pComp] : components) {
      if (pComp->Get_ComponentType() != COMPONENT_TYPE::TRANSFORM)
        ++count;
    }
    ImGui::Text("Components (%zu)", count);

    for (auto &[id, pComp] : components) {
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
// GameObjectPropertiesGUI: GameObject RTTR 프로퍼티 직접 열거
//   - LayerNames / TagNames 등 vector<wstring> 다중 선택 드롭다운
// ====================================================
void Inspector::GameObjectPropertiesGUI(const Shared<GameObject> &pObj) {
  if (!pObj)
    return;

  if (!ImGui::CollapsingHeader("GameObject Properties",
                               ImGuiTreeNodeFlags_DefaultOpen))
    return;

  ImGui::PushID(static_cast<int>(pObj->Get_ObjectID()));

  auto rttrType = rttr::type::get(*pObj);
  for (auto &prop : rttrType.get_properties()) {
    string propName = prop.get_name().to_string();
    rttr::variant val = prop.get_value(*pObj);

    if (!val.is_type<std::vector<std::wstring>>())
      continue; // LayerNames / TagNames만 처리

    std::vector<std::wstring> currentNames =
        val.get_value<std::vector<std::wstring>>();

    // Combo 미리보기 텍스트
    string comboPreview = "Multiple...";
    if (currentNames.empty()) {
      comboPreview = "None";
    } else if (currentNames.size() == 1) {
      char buf[256] = {};
      wcstombs_s(nullptr, buf, currentNames[0].c_str(), sizeof(buf));
      comboPreview = buf;
    }

    if (ImGui::BeginCombo(propName.c_str(), comboPreview.c_str())) {
      // Registry 에서 전체 이름 목록 가져오기
      vector<wstring> allNames;
      if (propName == "LayerNames") {
        for (const auto &pair :
             GAME_INSTANCE->Get_LayerRegister()->Get_AllLayers()) {
          if (!pair.second.empty())
            allNames.push_back(pair.second);
        }
      } else if (propName == "TagNames") {
        for (const auto &pair :
             GAME_INSTANCE->Get_TagRegister()->Get_AllTags()) {
          if (!pair.second.empty())
            allNames.push_back(pair.second);
        }
      }

      for (const auto &regName : allNames) {
        auto it = std::find(currentNames.begin(), currentNames.end(), regName);
        bool isSelected = (it != currentNames.end());

        char labelBuf[256] = {};
        wcstombs_s(nullptr, labelBuf, regName.c_str(), sizeof(labelBuf));

        // DontClosePopups → 다중 체크 지원
        if (ImGui::Selectable(labelBuf, isSelected,
                              ImGuiSelectableFlags_DontClosePopups)) {
          if (isSelected)
            currentNames.erase(
                std::find(currentNames.begin(), currentNames.end(), regName));
          else
            currentNames.push_back(regName);

          prop.set_value(*pObj, currentNames);
        }
      }

      ImGui::EndCombo();
    }
  }

  ImGui::PopID();
}

// ====================================================
// ComponentGUI: 개별 컴포넌트 헤더 + RTTR 프로퍼티 표시
// ====================================================
void Inspector::ComponentGUI(const string &label,
                             const Shared<Component> &pComp) {
  if (!pComp)
    return;

  ImGui::PushID(static_cast<int>(pComp->Get_ObjectID()));

  if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
    auto rttrType = rttr::type::get(*pComp);
    for (auto &prop : rttrType.get_properties()) {
      string propName = prop.get_name().to_string();
      rttr::variant value = prop.get_value(*pComp);

      if (value.is_type<float>()) {
        float v = value.get_value<float>();
        if (ImGui::DragFloat(propName.c_str(), &v, 0.01f))
          prop.set_value(*pComp, v);
      } else if (value.is_type<Vector3>()) {
        Vector3 v = value.get_value<Vector3>();
        float arr[3] = {v.x, v.y, v.z};
        if (ImGui::DragFloat3(propName.c_str(), arr, 0.1f)) {
          v = Vector3(arr[0], arr[1], arr[2]);
          prop.set_value(*pComp, v);

          // Transform 이면 월드 행렬 갱신
          if (rttrType.is_derived_from<Transform>())
            static_pointer_cast<Transform>(pComp)->Update_WorldMatrix();
        }
      } else if (value.is_type<int>() || value.is_type<int32_t>()) {
        int v = value.get_value<int>();
        if (ImGui::DragInt(propName.c_str(), &v))
          prop.set_value(*pComp, v);
      } else if (value.is_type<bool>()) {
        bool v = value.get_value<bool>();
        if (ImGui::Checkbox(propName.c_str(), &v))
          prop.set_value(*pComp, v);
      } else {
        // 지원하지 않는 타입은 타입명만 표시
        ImGui::TextDisabled("  %s : <%s>", propName.c_str(),
                            value.get_type().get_name().to_string().c_str());
      }
    }
  }

  ImGui::PopID();
}

Shared<Inspector> Inspector::Create() {
  auto instance = make_shared<Inspector>();

  if (FAILED(instance->Initialize())) {
    MSG_BOX("Failed To Create Inspector");
    return nullptr;
  }

  return instance;
}
