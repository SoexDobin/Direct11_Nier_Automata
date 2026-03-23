#include "pch.h"
#include "InspectorModel.h"
#include "EditorManager.h"
#include "Model.h"
#include "String_Helper.h"

using namespace Editor;
using namespace Engine;

HRESULT InspectorModel::Initialize()
{
    return S_OK;
}

void InspectorModel::RenderComponent(const Shared<Component>& pComp)
{
	auto pModel = static_pointer_cast<Model>(pComp);
	if (!pModel) return;

    if (ImGui::CollapsingHeader("Model Component", ImGuiTreeNodeFlags_DefaultOpen))
    {
        wstring currentTag = pModel->Get_ModelTag();
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
                        pModel->Set_ModelTag(Helper::To_wString(droppedTag));
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);

        ImGui::Spacing();
    }
}

Shared<InspectorModel> InspectorModel::Create()
{
	auto instance = make_shared<InspectorModel>();
	if (FAILED(instance->Initialize())) return nullptr;
	return instance;
}
