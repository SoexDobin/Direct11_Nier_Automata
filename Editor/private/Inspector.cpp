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
#include "Engine_RTTR_Metadata.h"

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
    std::string saveDataStr{};
    
    rttr::variant metaWidget = prop.get_metadata(Meta_Key::Widget);
    if (metaWidget.is_valid()) widgetStr = metaWidget.to_string();

    rttr::variant metaSave = prop.get_metadata(Meta_Key::SaveData);
    if (metaSave.is_valid()) saveDataStr = metaSave.to_string();

    Bool isReadOnly = prop.is_readonly();
    if (isReadOnly) ImGui::BeginDisabled();
    
    // [Diagnostic] 마우스 오버 시 메타데이터 표시 (Ctrl 키 누를 때만)
    if (ImGui::IsItemHovered() && ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
    {
        ImGui::BeginTooltip();
        ImGui::Text("Prop: %s", propName.c_str());
        ImGui::Text("Widget: %s", widgetStr.c_str());
        ImGui::Text("SaveData: %s", saveDataStr.c_str());
        ImGui::EndTooltip();
    }

    // --- 1. 우선순위: 위젯 메타데이터 기반 처리 (AssetDrop, GameObject 등) ---
    
    // [AssetDrop] 모델, 텍스처 등 에셋 태그 등록용 슬롯
    if (widgetStr == "AssetDrop" || saveDataStr == "TextureTag" || saveDataStr == "ModelTag" || propName.find("Tag") != string::npos)
    {
        wstring wTag{};
        if (varValue.is_type<wstring>()) wTag = varValue.get_value<wstring>();
        else if (varValue.is_type<string>()) wTag = Helper::To_wString(varValue.get_value<string>());

        string sTag = Helper::To_String(wTag);
        string requiredType = prop.get_metadata(Meta_Key::AssetType).to_string();
        if (requiredType.empty())
        {
            if (saveDataStr == "TextureTag" || propName.find("Texture") != string::npos) requiredType = "Texture";
            else if (saveDataStr == "ModelTag" || propName.find("Model") != string::npos) requiredType = "Model";
        }

        ImGui::Text(propName.c_str());
        ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.3f);

        // --- 드롭 슬롯 렌더링 ---
        ImVec2 slotSize = ImVec2(ImGui::GetContentRegionAvail().x - 5.f, 25.f);
        
        ImVec4 slotColor = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
        ImVec4 textColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
        
        if (const ImGuiPayload* payload = ImGui::GetDragDropPayload())
        {
            if (string(payload->DataType) == "ASSET_BROWSER_ITEM")
            {
                string payloadStr = static_cast<const Char*>(payload->Data);
                
                if (payloadStr.find(requiredType + "|") == 0) 
                {
                    slotColor = ImVec4(0.1f, 0.4f, 0.1f, 1.0f); // Compatible: Green
                    textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                }
                else 
                {
                    slotColor = ImVec4(0.4f, 0.1f, 0.1f, 1.0f); // Incompatible: Red
                }
            }
        }

        ImGui::PushStyleColor(ImGuiCol_Header, slotColor);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(slotColor.x * 1.2f, slotColor.y * 1.2f, slotColor.z * 1.2f, slotColor.w));
        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
        
        string displayText = sTag.empty() ? "(None / Drag " + requiredType + " here)" : sTag;
        string selectID = displayText + "##" + propName;
        
        if (ImGui::Selectable(selectID.c_str(), true, ImGuiSelectableFlags_None, slotSize)) { }
        
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_BROWSER_ITEM"))
            {
                string payloadStr = static_cast<const Char*>(payload->Data);
                size_t delimPos = payloadStr.find('|');
                if (delimPos != string::npos)
                {
                    string droppedType = payloadStr.substr(0, delimPos);
                    string droppedTag = payloadStr.substr(delimPos + 1);

                    if (requiredType.empty() || requiredType == droppedType)
                    {
                        if (varValue.is_type<wstring>()) prop.set_value(instance, Helper::To_wString(droppedTag));
                        else prop.set_value(instance, droppedTag);
                        LOG_INFO("Asset Assigned: {} -> {}", propName, droppedTag);
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
        
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag & Drop %s from Asset Browser", requiredType.c_str());
        return;
    }
    // [GameObject] 객체 참조(Target)용 슬롯
    if (widgetStr == "GameObject" || saveDataStr == "GameObject" || propName.find("Target") != string::npos)
    {
        uint32 currentID = 0;
        if (varValue.is_type<uint32>()) currentID = varValue.get_value<uint32>();
        else if (varValue.is_type<int32>()) currentID = static_cast<uint32>(varValue.get_value<int32>());

        string targetName = "None";
        Bool isLinked = false;
        if (currentID != 0)
        {
            // [Engine] Game::Find_ObjectByObjectID가 이제 내부적으로 Level 0과 현재 레벨을 모두 검색함
            Shared<GameObject> targetObj = GAME_INSTANCE->Find_ObjectByObjectID(GAME_INSTANCE->Get_CurrentLevelIndex(), currentID);
            if (targetObj) 
            { 
                targetName = Helper::To_String(targetObj->Get_Name()); 
                isLinked = true; 
            }
            if (!isLinked) targetName = "Missing (ID:" + to_string(currentID) + ")";
        }

        ImGui::Text(propName.c_str());
        ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.3f);

        ImVec2 slotSize = ImVec2(ImGui::GetContentRegionAvail().x - 35.f, 25.f);
        
        ImVec4 slotColor = isLinked ? ImVec4(0.1f, 0.25f, 0.35f, 1.0f) : ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
        ImVec4 textColor = isLinked ? ImVec4(0.6f, 0.9f, 1.0f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

        if (const ImGuiPayload* payload = ImGui::GetDragDropPayload())
        {
            if (string(payload->DataType) == ObjectMove_PayLoadKey) 
            {
                slotColor = ImVec4(0.2f, 0.5f, 0.6f, 1.0f); // Highlighting on drag
                textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
        }

        ImGui::PushStyleColor(ImGuiCol_Header, slotColor);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(slotColor.x * 1.2f, slotColor.y * 1.2f, slotColor.z * 1.2f, slotColor.w));
        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
        
        string selectID = targetName + "##" + propName;
        if (ImGui::Selectable(selectID.c_str(), true, ImGuiSelectableFlags_None, slotSize)) {  }
        
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ObjectMove_PayLoadKey.c_str()))
            {
                uint32 droppedInstanceID = *(uint32*)payload->Data;
                // [Engine] Game::Find_ByInstanceID가 이제 내부적으로 Level 0과 현재 레벨을 모두 검색함
                Shared<GameObject> droppedObj = GAME_INSTANCE->Find_ByInstanceID(GAME_INSTANCE->Get_CurrentLevelIndex(), droppedInstanceID);
                
                if (droppedObj)
                {
                    prop.set_value(instance, droppedObj->Get_ObjectID());
                    LOG_INFO("Target Linked: {} -> {} (ObjectID: {})", propName, Helper::To_String(droppedObj->Get_Name()), droppedObj->Get_ObjectID());
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        if (ImGui::Button(("X##" + propName).c_str(), ImVec2(30, 25))) prop.set_value(instance, 0u);
        return;
    }
    // --- 2. 일반 타입별 위젯 처리 ---
    if (varValue.is_type<Engine::Vector3>())
    {
        Vector3 vec = varValue.get_value<Engine::Vector3>();
        Float speed = 0.1f;
        auto metaSpeed = prop.get_metadata(Meta_Key::Speed);
        if (metaSpeed.is_valid() && metaSpeed.can_convert<Float>()) speed = metaSpeed.to_float();
        if (ImGui::DragFloat3((propName + "##" + propName).c_str(), reinterpret_cast<Float*>(&vec), speed)) prop.set_value(instance, vec);
    }
    else if (widgetStr == Widget_Type::ColorPicker || varValue.is_type<Color>())
    {
        Color col = varValue.get_value<Color>();
        if (ImGui::ColorEdit4((propName + "##" + propName).c_str(), reinterpret_cast<Float*>(&col))) prop.set_value(instance, col);
    }
    else if (widgetStr == "SliderFloat" || widgetStr == Widget_Type::SliderFloat)
    {
        if (varValue.is_type<Float>())
        {
            Float val = varValue.get_value<Float>();
            Float minVal = 0.f, maxVal = 100.f;
            auto metaMin = prop.get_metadata(Meta_Key::Min);
            auto metaMax = prop.get_metadata(Meta_Key::Max);
            if (metaMin.is_valid() && metaMin.can_convert<Float>()) minVal = metaMin.to_float();
            if (metaMax.is_valid() && metaMax.can_convert<Float>()) maxVal = metaMax.to_float();
            if (ImGui::SliderFloat((propName + "##" + propName).c_str(), &val, minVal, maxVal)) prop.set_value(instance, val);
        }
    }
    else if (varValue.is_type<float>())
    {
        float val = varValue.get_value<float>();
        if (ImGui::DragFloat((propName + "##" + propName).c_str(), &val, 0.1f)) prop.set_value(instance, val);
    }
    else if (varValue.is_type<int>() || varValue.is_type<int32>() || varValue.is_type<uint32>())
    {
        int val = varValue.convert<int>();
        if (ImGui::DragInt((propName + "##" + propName).c_str(), &val)) prop.set_value(instance, val);
    }
    else if (varValue.is_type<bool>())
    {
        bool val = varValue.get_value<bool>();
        if (ImGui::Checkbox((propName + "##" + propName).c_str(), &val)) prop.set_value(instance, val);
    }
    else if (varValue.is_type<std::string>() || varValue.is_type<std::wstring>())
    {
        string sVal = varValue.is_type<string>() ? varValue.get_value<string>() : Helper::To_String(varValue.get_value<wstring>());
        ImGui::LabelText(propName.c_str(), "%s", sVal.c_str());
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
