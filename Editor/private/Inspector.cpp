#include "pch.h"
#include "Editor_Define.h"
#include "Inspector.h"
#include "EditorManager.h"
#include "Component.h"
#include "GameObject.h"
#include "Game.h"
#include "LayerRegistry.h"
#include "PathManager.h"
#include "TagRegistry.h"
#include "Transform.h"
#include "Engine_RTTR_Metadata.h"
#include "InspectorTransform.h"
#include "InspectorModel.h"
#include "InspectorTexture.h"
#include "InspectorCamera.h"
#include "InspectorCollider.h"

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
    m_InspectorCollider = InspectorCollider::Create();

    return EditorObject::Initialize();
}

void Inspector::Render(Bool isResize) {
    ImGui::Begin("Inspector");

    Render_ProjectSettingsWindow();

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

void Inspector::Render_ProjectSettingsWindow()
{
    auto layerReg = GAME_INSTANCE->Get_LayerRegister();
    auto tagReg = GAME_INSTANCE->Get_TagRegister();
    // JSON 원본 문자열을 UI 버퍼에 단 한 번만 복사
    if (m_IsFirstLoadSettings)
    {
        for (const auto& pair : layerReg->Get_AllLayers()) {
            uint32 bit = static_cast<uint32>(pair.first);
            int index = 0; while ((bit >>= 1) > 0) index++;
            strcpy_s(m_LayerNames[index], 64, Helper::To_String(pair.second).c_str());
        }
        for (const auto& pair : tagReg->Get_AllTags()) {
            uint32 bit = static_cast<uint32>(pair.first);
            int index = 0; while ((bit >>= 1) > 0) index++;
            strcpy_s(m_TagNames[index], 64, Helper::To_String(pair.second).c_str());
        }
        m_IsFirstLoadSettings = false;
    }
    // ⭐️ 헤더(드롭다운)로 묶어서 공간 낭비 방지 ⭐️
    if (ImGui::CollapsingHeader("Global Layer & Tag Settings", ImGuiTreeNodeFlags_None))
    {
        // ⭐️ 스크롤을 내려도 항상 콤보박스(헤더) 바로 아래에서 세이브가 가능하도록 상단에 버튼 고정 ⭐️
        ImGui::Columns(2, "SaveButtons", false);
        if (ImGui::Button("Save Layers JSON", ImVec2(-1, 0))) layerReg->SaveToFile(PATH.GetLayerSettingsPath());
        ImGui::NextColumn();
        if (ImGui::Button("Save Tags JSON", ImVec2(-1, 0))) tagReg->SaveToFile(PATH.GetTagSettingsPath());
        ImGui::Columns(1);

        ImGui::Separator();
        // ⭐️ 오버플로우 방지 및 스크롤 영역 (높이 250) ⭐️
        ImGui::BeginChild("SettingsScrollArea", ImVec2(0, 250), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        // ⭐️ 가독성 개선: 좌(Layer) / 우(Tag) 양분 ⭐️
        ImGui::Columns(2, "LayerTagColumns", true); // 리사이즈 가능 선(true)
        // [왼쪽 컬럼: Layers]
        ImGui::TextDisabled("=== Layers ===");
        for (int i = 0; i < 32; ++i)
        {
            ImGui::Text("L%02d", i);
            ImGui::SameLine(35.f); // 텍스트박스 시작 지점을 가지런하게 맞춤
            ImGui::PushItemWidth(-1.f); // 텍스트박스를 끝까지 채움
            string label = "##Layer" + std::to_string(i);
            if (ImGui::InputText(label.c_str(), m_LayerNames[i], 64))
                layerReg->Set_LayerName(1 << i, Helper::To_wString(m_LayerNames[i]));
            ImGui::PopItemWidth();
        }
        ImGui::NextColumn(); // 다음 열로 이동
        // [오른쪽 컬럼: Tags]
        ImGui::TextDisabled("=== Tags ===");
        for (int i = 0; i < 32; ++i)
        {
            ImGui::Text("T%02d", i);
            ImGui::SameLine(35.f);
            ImGui::PushItemWidth(-1.f);
            string label = "##Tag" + std::to_string(i);
            if (ImGui::InputText(label.c_str(), m_TagNames[i], 64))
                tagReg->Set_TagName(1 << i, Helper::To_wString(m_TagNames[i]));
            ImGui::PopItemWidth();
        }
        // 범위 초과(넘치면) 자동 스크롤(슬라이더) 제공 구역 종료
        ImGui::Columns(1);
        ImGui::EndChild();
    }
    // 인스펙터 하단 오브젝트 정보와 분리선 출력
    ImGui::Separator();
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
            else if ((cType == COMPONENT_TYPE::OBB_COLLIDER || cType == COMPONENT_TYPE::SPHERE_COLLIDER) && m_InspectorCollider)
            {
                m_InspectorCollider->RenderComponent(pComp);
            }
            else {
                string typeName = GAME_INSTANCE->Find_RegisteredName(pComp->Get_RuntimeTypeId());
				if (typeName.empty())
					typeName = "<Unregistered Component>";
                if (ImGui::CollapsingHeader(typeName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
					RenderGenericProperties(*pComp);
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
            string typeName = GAME_INSTANCE->Find_RegisteredName(pScript->Get_RuntimeTypeId());
			if (typeName.empty())
				typeName = "<Unregistered Script>";
            if (ImGui::CollapsingHeader(typeName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
				RenderGenericProperties(*pScript);
            }
        }
    }
}

void Inspector::RenderGenericProperties(Object& object)
{
	const vector<ReflectedPropertyInfo> properties =
		GAME_INSTANCE->Get_ReflectedProperties(object);
	if (properties.empty()) {
		ImGui::TextDisabled("No reflected properties.");
		return;
	}

	for (const ReflectedPropertyInfo& property : properties) {
		ReflectionValue value;
		if (FAILED(GAME_INSTANCE->Read_ReflectedProperty(
			object, property.registeredName, value))) {
			ImGui::TextDisabled("%s: unavailable", property.registeredName.c_str());
			continue;
		}

		ImGui::PushID(property.registeredName.c_str());
		Bool changed = false;
		switch (property.valueType) {
		case REFLECTION_VALUE_TYPE::BOOL:
			if (Bool* typed = value.Try_Get<Bool>())
				changed = ImGui::Checkbox(property.registeredName.c_str(), typed);
			break;
		case REFLECTION_VALUE_TYPE::INT32:
			if (int32* typed = value.Try_Get<int32>())
				changed = ImGui::InputInt(property.registeredName.c_str(), typed);
			break;
		case REFLECTION_VALUE_TYPE::UINT32:
			if (uint32* typed = value.Try_Get<uint32>())
				changed = ImGui::InputScalar(property.registeredName.c_str(),
					ImGuiDataType_U32, typed);
			break;
		case REFLECTION_VALUE_TYPE::FLOAT:
			if (Float* typed = value.Try_Get<Float>())
				changed = ImGui::DragFloat(property.registeredName.c_str(), typed, 0.01f);
			break;
		case REFLECTION_VALUE_TYPE::DOUBLE:
			if (Double* typed = value.Try_Get<Double>())
				changed = ImGui::InputDouble(property.registeredName.c_str(), typed);
			break;
		case REFLECTION_VALUE_TYPE::STRING:
			if (string* typed = value.Try_Get<string>()) {
				Char buffer[256]{};
				strncpy_s(buffer, typed->c_str(), _TRUNCATE);
				if (ImGui::InputText(property.registeredName.c_str(), buffer, sizeof(buffer))) {
					*typed = buffer;
					changed = true;
				}
			}
			break;
		case REFLECTION_VALUE_TYPE::WSTRING:
			if (wstring* typed = value.Try_Get<wstring>()) {
				Char buffer[256]{};
				const string utf8 = Helper::To_String(*typed);
				strncpy_s(buffer, utf8.c_str(), _TRUNCATE);
				if (ImGui::InputText(property.registeredName.c_str(), buffer, sizeof(buffer))) {
					*typed = Helper::To_wString(buffer);
					changed = true;
				}
			}
			break;
		case REFLECTION_VALUE_TYPE::VECTOR3:
			if (Vector3* typed = value.Try_Get<Vector3>())
				changed = ImGui::DragFloat3(property.registeredName.c_str(),
					reinterpret_cast<Float*>(typed), 0.01f);
			break;
		case REFLECTION_VALUE_TYPE::FLOAT3:
			if (Float3* typed = value.Try_Get<Float3>())
				changed = ImGui::DragFloat3(property.registeredName.c_str(),
					reinterpret_cast<Float*>(typed), 0.01f);
			break;
		case REFLECTION_VALUE_TYPE::COLOR:
			if (Color* typed = value.Try_Get<Color>())
				changed = ImGui::ColorEdit4(property.registeredName.c_str(),
					reinterpret_cast<Float*>(typed));
			break;
		case REFLECTION_VALUE_TYPE::FLOAT4:
			if (Float4* typed = value.Try_Get<Float4>())
				changed = ImGui::DragFloat4(property.registeredName.c_str(),
					reinterpret_cast<Float*>(typed), 0.01f);
			break;
		case REFLECTION_VALUE_TYPE::ANIMATION_PRESET:
			if (const AnimationPresetSnapshot* typed = value.Try_Get<AnimationPresetSnapshot>())
				ImGui::Text("%s: %s (%zu clips)", property.registeredName.c_str(),
					typed->animationEnum.empty() ? "<None>" : typed->animationEnum.c_str(),
					typed->animations.size());
			break;
		default:
			ImGui::TextDisabled("%s: unsupported value type",
				property.registeredName.c_str());
			break;
		}

		if (changed && property.isWritable &&
			FAILED(GAME_INSTANCE->Write_ReflectedProperty(
				object, property.registeredName, value))) {
			ImGui::TextDisabled("Write rejected; value was not changed.");
		}
		ImGui::PopID();
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
