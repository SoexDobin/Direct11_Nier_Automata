#include "pch.h"
#include "Editor_Define.h"
#include "Inspector.h"
#include "EditorManager.h"
#include "Component.h"
#include "GameObject.h"
#include "LayerRegistry.h"
#include "PathManager.h"
#include "TagRegistry.h"
#include "Transform.h"

using namespace Engine;
using namespace Editor;

Inspector::Inspector() {}
Inspector::~Inspector() {}

HRESULT Inspector::Initialize() {
  auto layerRegistry = GAME_INSTANCE->Get_LayerRegister();
  auto tagRegistry = GAME_INSTANCE->Get_TagRegister();

  layerRegistry->LoadFromFile(PATH.GetLayerSettingsPath());
  tagRegistry->LoadFromFile(PATH.GetTagSettingsPath());

  return EditorObject::Initialize();
}

void Inspector::Render(Bool isResize) {
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
	
	string name = Helper::To_String(Clean_RTTR_Name(pObj->Get_Name()));
	ImGui::Text("Name : %s", name.c_str());
	ImGui::Text("ID   : %u", pObj->Get_InstanceID());
	ImGui::Separator();

	/*GameObject*/
	GameObjectPropertiesGUI(pObj);
	ImGui::Separator();

	/*Transform*/
	Shared<Transform> transform = pObj->Get_Transform();
    if (transform) {
      string typeName = rttr::type::get(*transform).get_name().to_string();
      ComponentGUI(typeName, transform);
    }
    ImGui::Separator();

	/*Camera (If applicable)*/
    auto cameraType = rttr::type::get_by_name("Camera");
    if (cameraType.is_valid() && rttr::type::get(*pObj).is_derived_from(cameraType))
    {
        if (ImGui::CollapsingHeader("Camera Properties", ImGuiTreeNodeFlags_DefaultOpen))
        {
            for (auto& prop : rttr::type::get(*pObj).get_properties())
            {
                // Camera 클래스(또는 상위 클래스 중 Camera)에서 선언된 값들만 표시
                if (prop.get_declaring_type() == cameraType)
                {
                    Render_Properties(prop, pObj);
                }
            }
        }
        ImGui::Separator();
    }

  // ── Components 섹션 (Transform 제외)
  const auto &components = pObj->Get_Components();
  if (!components.empty()) {
    // Transform 은 위에서 이미 그렸으니 카운트 제외
    size_t count = 0;
    for (auto &pComp : components) {
      if (pComp->Get_ComponentType() != COMPONENT_TYPE::TRANSFORM)
        ++count;
    }
    ImGui::Text("Components (%zu)", count);

    for (auto& pComp : components) {
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
    for (auto& pScript : scripts) {
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

void Inspector::ComponentGUI(const string& label, const Shared<Component>& component) {
    if (!component) return;
    ImGui::PushID(component->Get_ObjectID());
    if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
        auto componentType = rttr::type::get(*component);
        for (auto& prop : componentType.get_properties()) {
            // prop와 컴포넌트 실체를 함께 넘김
            Render_Properties(prop, component);
        }
    }
    ImGui::PopID();
}

void Inspector::Render_Properties(rttr::property prop, rttr::instance instance)
{
    std::string propName = prop.get_name().to_string();
    rttr::variant varValue = prop.get_value(instance);
    if (!varValue.is_valid())
        return;

    /* 메타데이터 파싱 (Widget 형태) */
    std::string widgetStr{};
    rttr::variant metaWidget = prop.get_metadata("SaveData");
    if (metaWidget.is_valid() && metaWidget.can_convert<std::string>())
    {
        widgetStr = metaWidget.to_string();
    }

    Bool isReadOnly = prop.is_readonly();
    if (isReadOnly) ImGui::BeginDisabled();
    
    if (varValue.is_type<Engine::Vector3>())
    {
        Vector3 vec = varValue.get_value<Engine::Vector3>();

        Float speed = 0.1f;
        auto metaSpeed = prop.get_metadata("Speed");
        if (metaSpeed.is_valid() && metaSpeed.can_convert<Float>()) speed = metaSpeed.to_float();

        if (ImGui::DragFloat3(propName.c_str(), reinterpret_cast<Float*>(&vec), speed))
        {
            if (!prop.set_value(instance, vec))
                LOG_WARN("Failed to Set : {}", propName);
        }
    }
    else if (widgetStr == "ColorPicker" || varValue.is_type<Color>())
    {
        Color col = varValue.get_value<Color>();
        if (ImGui::ColorEdit4(propName.c_str(), reinterpret_cast<float*>(&col)))
        {
            if (!prop.set_value(instance, col))
                LOG_WARN("Failed to Set : {}", propName);
        }
    }
    else if (widgetStr == "SliderFloat" && varValue.is_type<Float>())
    {
        Float val = varValue.get_value<Float>();
        Float minVal = 0.f, maxVal = 100.f;

        auto metaMin = prop.get_metadata("Min");
        auto metaMax = prop.get_metadata("Max");
        if (metaMin.is_valid() && metaMin.can_convert<Float>()) minVal = metaMin.to_float();
        if (metaMax.is_valid() && metaMax.can_convert<Float>()) maxVal = metaMax.to_float();

        if (ImGui::SliderFloat(propName.c_str(), &val, minVal, maxVal))
        {
            if (!prop.set_value(instance, val))
                LOG_WARN("Failed to Set : {}", propName);
        }
    }
    else if (widgetStr == "SliderInt" && varValue.is_type<int32>())
    {
        int32 val = varValue.get_value<int32>();
        int32 minVal = 0, maxVal = 100;

        auto metaMin = prop.get_metadata("Min");
        auto metaMax = prop.get_metadata("Max");
        if (metaMin.is_valid() && metaMin.can_convert<int32>()) minVal = metaMin.to_int();
        if (metaMax.is_valid() && metaMax.can_convert<int32>()) maxVal = metaMax.to_int();

        if (ImGui::SliderInt(propName.c_str(), &val, minVal, maxVal))
        {
            if (!prop.set_value(instance, val))
                LOG_WARN("Failed to Set : {}", propName);
        }
    }
    else if (varValue.is_type<int>())
    {
        int val = varValue.get_value<int>();
        if (ImGui::DragInt(propName.c_str(), &val)) prop.set_value(instance, val);
    }
    else if (varValue.is_type<float>())
    {
        float val = varValue.get_value<float>();
        if (ImGui::DragFloat(propName.c_str(), &val, 0.1f)) prop.set_value(instance, val);
    }
    else if (varValue.is_type<bool>())
    {
        bool val = varValue.get_value<bool>();
        if (ImGui::Checkbox(propName.c_str(), &val)) prop.set_value(instance, val);
    }
    else if (varValue.is_type<std::string>())
    {
        std::string valStr = varValue.get_value<std::string>();
        ImGui::LabelText(propName.c_str(), "%s", valStr.c_str());
    }
    else if (widgetStr == "AssetDrop")
    {
        // wstring 안전 변환 시도
        wstring wTag{};
        if (varValue.can_convert<wstring>())
            wTag = varValue.get_value<wstring>();
        else if (varValue.can_convert<std::wstring>())
            wTag = varValue.get_value<std::wstring>();

        string sTag = Helper::To_String(wTag);
        
        ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, 25);
        ImGui::Button(sTag.empty() ? "(Empty Asset)" : sTag.c_str(), size);

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_BROWSER_ITEM"))
            {
                // 페이로드 구조: "Type|Tag"
                string payloadStr = (const char*)payload->Data;
                size_t delimPos = payloadStr.find('|');
                if (delimPos != string::npos)
                {
                    string droppedType = payloadStr.substr(0, delimPos);
                    string droppedTag = payloadStr.substr(delimPos + 1);
                    
                    // RTTR 메타데이터의 AssetType과 일치하는지 확인
                    rttr::variant metaType = prop.get_metadata("AssetType");
                    string requiredType = metaType.is_valid() ? metaType.to_string() : "";

                    if (requiredType == droppedType)
                    {
                        prop.set_value(instance, Helper::To_wString(droppedTag));
                        LOG_INFO("Asset Updated: {} ({})", droppedTag, droppedType);
                    }
                    else
                    {
                        LOG_WARN("Asset Type Mismatch! Required: {}, Dropped: {}", requiredType, droppedType);
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::SameLine();
        ImGui::Text(propName.c_str());
    }
    else if (widgetStr == "GameObject")
    {
        uint32 currentID = 0;
        rttr::variant var = prop.get_value(instance);
        if (var.is_type<uint32>()) currentID = var.get_value<uint32>();
        else if (var.is_type<int>()) currentID = static_cast<uint32>(var.get_value<int>());

        // 1. 현재 타겟 정보 확보
        string targetName = "None";
        bool isLinked = false;
        if (currentID != 0)
        {
            Shared<GameObject> targetObj = GAME_INSTANCE->Find_ObjectByObjectID(currentID);
            if (targetObj)
            {
                targetName = Helper::To_String(targetObj->Get_Name());
                isLinked = true;
            }
            else targetName = "Missing (" + to_string(currentID) + ")";
        }

        // 2. 에셋 슬롯 스타일 UI 렌더링
        ImGui::Text(propName.c_str());
        ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);

        // 슬롯 배경/테두리 스타일
        ImVec2 slotSize = ImVec2(ImGui::GetContentRegionAvail().x - 30.f, 20.f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, isLinked ? ImVec4(0.2f, 0.4f, 0.6f, 1.0f) : ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
        
        // InputText를 사용하여 이름 표시 (Assertion 방지를 위해 고정 버퍼 사용)
        char buf[256];
        strncpy_s(buf, targetName.c_str(), sizeof(buf));
        ImGui::SetNextItemWidth(slotSize.x);
        ImGui::InputText(("##" + propName).c_str(), buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);
        
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ObjectMove_PayLoadKey.c_str()))
            {
                uint32 droppedInstanceID = *(uint32*)payload->Data;
                Shared<GameObject> droppedObj = GAME_INSTANCE->Find_ByInstanceID(droppedInstanceID);
                if (droppedObj)
                {
                    uint32 droppedObjectID = droppedObj->Get_ObjectID();
                    prop.set_value(instance, droppedObjectID);
                    LOG_INFO("GameObject Linked: {} (ObjectID: {})", propName, droppedObjectID);
                }
                else
                {
                    LOG_WARN("Failed to Link GameObject: Invalid InstanceID.");
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::PopStyleColor();

        // 3. 지우기 버튼 (X)
        ImGui::SameLine();
        if (ImGui::Button(("X##" + propName).c_str(), ImVec2(20, 20)))
        {
            prop.set_value(instance, 0u);
        }
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Clear Reference");
    }

    // --- 읽기 전용 해제 ---
    if (isReadOnly) ImGui::EndDisabled();
}


Shared<Inspector> Inspector::Create() {
  auto instance = make_shared<Inspector>();

  if (FAILED(instance->Initialize())) {
    MSG_BOX("Failed To Create Inspector");
    return nullptr;
  }

  return instance;
}
