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
#include "InspectorTransform.h"
#include "InspectorModel.h"
#include "InspectorTexture.h"
#include "InspectorCamera.h"

using namespace Engine;
using namespace Editor;

Inspector::Inspector() {}
Inspector::~Inspector() {}

HRESULT Inspector::Initialize() {
    auto layerRegistry = GAME_INSTANCE->Get_LayerRegister();
    auto tagRegistry = GAME_INSTANCE->Get_TagRegister();

    layerRegistry->LoadFromFile(PATH.GetLayerSettingsPath());
    tagRegistry->LoadFromFile(PATH.GetTagSettingsPath());

    m_InspectorTransform = InspectorTransform::Create();
    m_InspectorModel = InspectorModel::Create();
    m_InspectorTexture = InspectorTexture::Create();
    m_InspectorCamera = InspectorCamera::Create();

    return EditorObject::Initialize();
}

string Clean_RTTR_Name(const wstring& name) {
    return Helper::To_String(name); // Optional: keep any custom old logic if needed
}

void Inspector::Render(Bool isResize) {
    ImGui::Begin("Inspector");

    Shared<GameObject> selected = EDITOR->Get_SelectedObject();
    if (!selected) {
		m_iPrevSelectedID = 0;
		ImGui::TextDisabled("No object selected.");
		ImGui::End();
		return;
    }

    if (m_iPrevSelectedID != selected->Get_ObjectID()) {
        m_iPrevSelectedID = selected->Get_ObjectID();
    }

    GameObjectGUI(selected);

    ImGui::End();
}

// ====================================================
// GameObjectGUI: 선택 오브젝트의 이름・Layer/Tag・Transform・컴포넌트 표시
// ====================================================
void Inspector::GameObjectGUI(const Shared<GameObject>& obj) {
    Draw_GameObjectHeader(obj);
    ImGui::Separator();

    // Transform 렌더링
    if (m_InspectorTransform) {
        m_InspectorTransform->RenderComponent(obj->Get_Transform());
    }

    ImGui::Separator();

    // Camera 렌더링 (Camera는 Component가 아니라 GameObject를 상속받음)
    if (obj->Get_GameObjectType() == GAMEOBJECTTYPE::CAMERA && m_InspectorCamera) {
        m_InspectorCamera->RenderCamera(obj);
        ImGui::Separator();
    }

    // 모델, 텍스처 등 기타 컴포넌트 렌더링
    const auto& components = obj->Get_Components();
    if (!components.empty()) {
        size_t count = 0;
        for (auto& pComp : components) {
            if (pComp && pComp->Get_ComponentType() != COMPONENT_TYPE::TRANSFORM)
                ++count;
        }
        ImGui::Text("Components (%zu)", count);

        for (auto& pComp : components) {
            // Transform은 위에서 그렸으니 패스
            if (!pComp || pComp->Get_ComponentType() == COMPONENT_TYPE::TRANSFORM)
                continue;

            COMPONENT_TYPE cType = pComp->Get_ComponentType();
            if (cType == COMPONENT_TYPE::MODEL && m_InspectorModel) {
                m_InspectorModel->RenderComponent(pComp);
            }
            else if (cType == COMPONENT_TYPE::TEXTURE && m_InspectorTexture) {
                m_InspectorTexture->RenderComponent(pComp);
            }
            else {
                // 커스텀 인스펙터가 없는 경우 Fallback
                string typeName = rttr::type::get(*pComp).get_name().to_string();
                if (ImGui::CollapsingHeader(typeName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::TextDisabled("No custom inspector available.");
                }
            }
        }
    }

    ImGui::Separator();

    // Scripts 섹션
    const auto& scripts = obj->Get_Scripts();
    if (!scripts.empty()) {
        ImGui::Text("Scripts (%zu)", scripts.size());
        for (auto& pScript : scripts) {
            if (!pScript) continue;
            string typeName = rttr::type::get(*pScript).get_name().to_string();
            if (ImGui::CollapsingHeader(typeName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::TextDisabled("No custom inspector for Script.");
            }
        }
    }
}

void Inspector::Draw_GameObjectHeader(const Shared<Engine::GameObject>& obj)
{
    // 이름
    string name = Helper::To_String(obj->Get_Name());
    ImGui::Text("Name : %s", name.c_str());
    ImGui::Text("ID   : %u", obj->Get_InstanceID());
    
    ImGui::Separator();

    auto layerRegistry = GAME_INSTANCE->Get_LayerRegister();
    auto tagRegistry = GAME_INSTANCE->Get_TagRegister();

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.6f);

    // 1. Layer (오브젝트의 현재 소속 레이어 - 보통 단일 비트)
    uint32 currentLayer = obj->Get_LayerMask().Get_Layer(); 
    string currentLayerName = Helper::To_String(layerRegistry->Get_LayerName(currentLayer));
    if (currentLayerName.empty()) currentLayerName = "Default";

    if (ImGui::BeginCombo("Layer", currentLayerName.c_str()))
    {
        for (const auto& pair : layerRegistry->Get_AllLayers())
        {
            uint32 layerBit = static_cast<uint32>(pair.first);
            if (layerBit == 0) continue; // Skip empty bit if any
            
            bool isSelected = (layerBit == currentLayer);
            string layerNameStr = Helper::To_String(pair.second);
            if (layerNameStr.empty()) continue;

            if (ImGui::Selectable(layerNameStr.c_str(), isSelected)) {
                obj->Get_LayerMask().Set_Layer(pair.first); 
            }
            if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    // 2. Layer Mask (오브젝트가 상호작용/충돌할 레이어 마스크 - 다중 비트)
    uint32 currentMask = obj->Get_LayerMask().Get_Mask();
    string maskPreview = (currentMask == 0xFFFFFFFF) ? "Everything" : ((currentMask == 0) ? "Nothing" : "Mixed...");

    if (ImGui::BeginCombo("Layer Mask", maskPreview.c_str()))
    {
        for (const auto& pair : layerRegistry->Get_AllLayers())
        {
            uint32 layerBit = static_cast<uint32>(pair.first);
            if (layerBit == 0) continue;
            
            bool isSelected = obj->Get_LayerMask().Has(layerBit);
            string layerNameStr = Helper::To_String(pair.second);
            if (layerNameStr.empty()) continue;

            if (ImGui::Selectable(layerNameStr.c_str(), isSelected, ImGuiSelectableFlags_DontClosePopups)) {
                if (isSelected) obj->Get_LayerMask().Remove(layerBit);
                else obj->Get_LayerMask().Add(layerBit);
            }
        }
        ImGui::EndCombo();
    }

    // 3. Tag (오브젝트가 가진 태그 - 다중 비트)
    uint32 currentTag = obj->Get_TagMask().Get_Tags();
    string tagPreview = (currentTag == 0) ? "Untagged" : "Mixed...";
    if (currentTag != 0) {
        // 단일 태그일 경우 이름을 보여주는 간단한 로직
        int count = 0;
        string singleName = "";
        for (const auto& pair : tagRegistry->Get_AllTags()) {
            if (obj->Get_TagMask().Has(static_cast<uint32>(pair.first))) {
                singleName = Helper::To_String(pair.second);
                count++;
            }
        }
        if (count == 1) tagPreview = singleName;
    }

    if (ImGui::BeginCombo("Tag##GO", tagPreview.c_str()))
    {
        for (const auto& pair : tagRegistry->Get_AllTags())
        {
            uint32 tagBit = static_cast<uint32>(pair.first);
            if (tagBit == 0) continue;

            bool isSelected = obj->Get_TagMask().Has(tagBit);
            string tagStr = Helper::To_String(pair.second);
            if (tagStr.empty()) continue;

            if (ImGui::Selectable(tagStr.c_str(), isSelected, ImGuiSelectableFlags_DontClosePopups)) {
                if (isSelected) obj->Get_TagMask().Remove(tagBit);
                else obj->Get_TagMask().Add(tagBit);
            }
        }
        ImGui::EndCombo();
    }
    
    ImGui::PopItemWidth();
}

Shared<Inspector> Inspector::Create() {
  auto instance = make_shared<Inspector>();
  if (FAILED(instance->Initialize())) {
    MSG_BOX("Failed To Create Inspector");
    return nullptr;
  }
  return instance;
}
