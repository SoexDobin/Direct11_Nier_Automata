#include "pch.h"
#include "InspectorModel.h"
#include "EditorManager.h"
#include "AnimationPresetEditor.h"
#include "Component.h"
#include "Game.h"
#include "PathManager.h"
#include "String_Helper.h"

using namespace Editor;
using namespace Engine;

namespace
{
	template <typename T>
	Bool ReadModelValue(Object& object, std::string_view propertyName, T& outValue)
	{
		ReflectionValue reflectedValue;
		if (FAILED(GAME_INSTANCE->Read_ReflectedProperty(object, propertyName, reflectedValue)))
			return false;
		const T* value = reflectedValue.Try_Get<T>();
		if (!value)
			return false;
		outValue = *value;
		return true;
	}

	template <typename T>
	Bool WriteModelValue(Object& object, std::string_view propertyName, const T& value)
	{
		ReflectionValue reflectedValue;
		reflectedValue.data = value;
		return SUCCEEDED(GAME_INSTANCE->Write_ReflectedProperty(
			object, propertyName, reflectedValue));
	}
}

HRESULT InspectorModel::Initialize()
{
    return S_OK;
}

void InspectorModel::RenderComponent(const Shared<Component>& pComp)
{
	if (!pComp) return;

    if (ImGui::CollapsingHeader("Model Component", ImGuiTreeNodeFlags_DefaultOpen))
    {
		wstring currentTag;
		ReadModelValue(*pComp, "ModelTag", currentTag);
        string sTag = Helper::To_String(currentTag);
        string requiredType = "Model"; // AssetTypeKey::Model

        ImGui::Text("ModelTag");
        ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.3f);
        
        ImVec2 slotSize = ImVec2(ImGui::GetContentRegionAvail().x - 5.f, 25.f);
        ImVec4 slotColor = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
        ImVec4 textColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
        
        if (const ImGuiPayload* payload = ImGui::GetDragDropPayload())
        {
            if (string(payload->DataType) == "ASSET_BROWSER_ITEM")
            {
                string payloadStr = static_cast<const Char*>(payload->Data);
                if (payloadStr.find(requiredType + "|") == 0) {
                    slotColor = ImVec4(0.1f, 0.4f, 0.1f, 1.0f); 
                    textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                }
                else {
                    slotColor = ImVec4(0.4f, 0.1f, 0.1f, 1.0f); 
                }
            }
        }

        ImGui::PushStyleColor(ImGuiCol_Header, slotColor);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(slotColor.x * 1.2f, slotColor.y * 1.2f, slotColor.z * 1.2f, slotColor.w));
        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
        
        string displayText = sTag.empty() ? "(None / Drag " + requiredType + " here)" : sTag;
        if (ImGui::Selectable((displayText + "##ModelTag").c_str(), true, ImGuiSelectableFlags_None, slotSize)) { }
        
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

                    if (requiredType == droppedType)
                    {
						WriteModelValue(*pComp, "ModelTag", Helper::To_wString(droppedTag));
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);

        ImGui::Spacing();
		ImGui::SeparatorText("Animation Preset Snapshot");
		AnimationPresetSnapshot appliedPreset;
		ReadModelValue(*pComp, "AnimationPreset", appliedPreset);
		ImGui::Text("Applied: %s", appliedPreset.Is_Empty()
			? "<None>" : appliedPreset.animationEnum.c_str());

		const string selectedPreset = m_SelectedPresetPath.empty()
			? "<Select Preset>" : Helper::To_String(m_SelectedPresetPath.stem().wstring());
		if (ImGui::BeginCombo("Preset", selectedPreset.c_str()))
		{
			const filesystem::path presetDirectory = PATH.GetAnimationPresetSettingsDir();
			std::error_code errorCode;
			if (filesystem::exists(presetDirectory, errorCode))
			{
				for (filesystem::directory_iterator it(presetDirectory,
					filesystem::directory_options::skip_permission_denied, errorCode), end;
					it != end; it.increment(errorCode))
				{
					if (errorCode) { errorCode.clear(); continue; }
					if (!it->is_regular_file() || it->path().extension() != L".json")
						continue;
					const string label = Helper::To_String(it->path().stem().wstring());
					if (ImGui::Selectable(label.c_str(), it->path() == m_SelectedPresetPath))
						m_SelectedPresetPath = it->path();
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::Button("Apply Preset Snapshot"))
		{
			string error;
			m_PresetStatus = SUCCEEDED(AnimationPresetEditor::Apply_PresetFile(
				*pComp, m_SelectedPresetPath, error))
				? "Preset snapshot applied; Scene/Prefab save will persist the full mapping."
				: "Preset apply failed: " + error;
		}
		if (!m_PresetStatus.empty())
			ImGui::TextWrapped("%s", m_PresetStatus.c_str());
    }
}

Shared<InspectorModel> InspectorModel::Create()
{
	auto instance = make_shared<InspectorModel>();
	if (FAILED(instance->Initialize())) return nullptr;
	return instance;
}
