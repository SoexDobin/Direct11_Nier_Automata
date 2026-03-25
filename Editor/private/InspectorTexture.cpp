#include "pch.h"
#include "InspectorTexture.h"
#include "EditorManager.h"
#include "Texture.h"
#include "String_Helper.h"

using namespace Editor;
using namespace Engine;

HRESULT InspectorTexture::Initialize()
{
    return S_OK;
}

void InspectorTexture::RenderComponent(const Shared<Component>& pComp)
{
	auto pTexture = static_pointer_cast<Texture>(pComp);
	if (!pTexture) return;

    if (ImGui::CollapsingHeader("Texture Component", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // 1. Color (RGBA)
        Color rgba = pTexture->Get_RGBAByValue();
        ImGui::Text("Color Multiplier");
        if (ImGui::ColorEdit4("##Color", reinterpret_cast<float*>(&rgba))) {
            pTexture->Set_RGBA(rgba);
        }
        
        // 2. ResourceTag (Texture Drag Drop)
        wstring currentTag = pTexture->Get_TextureTag();
        string sTag = Helper::To_String(currentTag);
        string requiredType = "Texture"; // AssetTypeKey::Texture

        ImGui::Text("TextureTag");
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
        if (ImGui::Selectable((displayText + "##TextureTag").c_str(), true, ImGuiSelectableFlags_None, slotSize)) { }
        
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
                        pTexture->Set_TextureTag(Helper::To_wString(droppedTag));
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

Shared<InspectorTexture> InspectorTexture::Create()
{
	auto instance = make_shared<InspectorTexture>();
	if (FAILED(instance->Initialize())) return nullptr;
	return instance;
}
