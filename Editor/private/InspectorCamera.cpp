#include "pch.h"
#include "InspectorCamera.h"
#include "Editor_Define.h"
#include "EditorManager.h"
#include "Game.h"
#include "GameObject.h"
#include "String_Helper.h"
#include "SpdLogger.h"

using namespace Editor;
using namespace Engine;

namespace
{
	template <typename T>
	Bool ReadCameraValue(Object& object, std::string_view propertyName, T& outValue)
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
	Bool WriteCameraValue(Object& object, std::string_view propertyName, const T& value)
	{
		ReflectionValue reflectedValue;
		reflectedValue.data = value;
		return SUCCEEDED(GAME_INSTANCE->Write_ReflectedProperty(
			object, propertyName, reflectedValue));
	}
}

HRESULT InspectorCamera::Initialize()
{
    return S_OK;
}

void InspectorCamera::RenderCamera(const Shared<GameObject>& pObj)
{
	if (!pObj) return;

    if (ImGui::CollapsingHeader("Camera Component", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // 1. FovY
        Float fovY{};
        if (ReadCameraValue(*pObj, "FovY", fovY) &&
			ImGui::SliderAngle("FovY", &fovY, 1.0f, 179.0f)) {
            WriteCameraValue(*pObj, "FovY", fovY);
        }

        // 2. Aspect
        Float aspect{};
        if (ReadCameraValue(*pObj, "Aspect", aspect) &&
			ImGui::DragFloat("Aspect", &aspect, 0.01f, 0.1f, 10.0f)) {
            WriteCameraValue(*pObj, "Aspect", aspect);
        }

        // 3. Near / Far
        Float nearPlane{};
        if (ReadCameraValue(*pObj, "Near", nearPlane) &&
			ImGui::DragFloat("Near", &nearPlane, 0.01f, 0.001f, 1000.0f)) {
            WriteCameraValue(*pObj, "Near", nearPlane);
        }

        Float farPlane{};
        if (ReadCameraValue(*pObj, "Far", farPlane) &&
			ImGui::DragFloat("Far", &farPlane, 1.0f, 1.0f, 10000.0f)) {
            WriteCameraValue(*pObj, "Far", farPlane);
        }
        
        // 4. TargetID (GameObject Drag & Drop)
        uint32 currentTargetID{};
		ReadCameraValue(*pObj, "TargetID", currentTargetID);
        string targetName = "None";
        bool isLinked = false;

        if (currentTargetID != 0)
        {
            Shared<GameObject> targetObj = GAME_INSTANCE->Find_ObjectByObjectID(GAME_INSTANCE->Get_CurrentLevelIndex(), currentTargetID);
            if (targetObj) {
                targetName = Helper::To_String(targetObj->Get_Name());
                isLinked = true;
            } else {
                targetName = "Missing (ID:" + std::to_string(currentTargetID) + ")";
            }
        }

        ImGui::Text("Target Object");
        ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.3f);
        
        ImVec2 slotSize = ImVec2(ImGui::GetContentRegionAvail().x - 35.f, 25.f);
        ImVec4 slotColor = isLinked ? ImVec4(0.1f, 0.25f, 0.35f, 1.0f) : ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
        ImVec4 textColor = isLinked ? ImVec4(0.6f, 0.9f, 1.0f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

        if (const ImGuiPayload* payload = ImGui::GetDragDropPayload())
        {
            if (string(payload->DataType) == ObjectMove_PayLoadKey) 
            {
                slotColor = ImVec4(0.2f, 0.5f, 0.6f, 1.0f);
                textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
        }

        ImGui::PushStyleColor(ImGuiCol_Header, slotColor);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(slotColor.x * 1.2f, slotColor.y * 1.2f, slotColor.z * 1.2f, slotColor.w));
        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
        
        string selectID = targetName + "##TargetID";
        if (ImGui::Selectable(selectID.c_str(), true, ImGuiSelectableFlags_None, slotSize)) { }
        
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ObjectMove_PayLoadKey.c_str()))
            {
                uint32 droppedInstanceID = *(uint32*)payload->Data;
                Shared<GameObject> droppedObj = GAME_INSTANCE->Find_ByInstanceID(GAME_INSTANCE->Get_CurrentLevelIndex(), droppedInstanceID);
                if (droppedObj)
                {
					WriteCameraValue(*pObj, "TargetID", droppedObj->Get_ObjectID());
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        if (ImGui::Button("X##TargetID", ImVec2(30, 25))) {
			WriteCameraValue(*pObj, "TargetID", uint32{});
        }

        ImGui::Spacing();
    }
}

Shared<InspectorCamera> InspectorCamera::Create()
{
	auto instance = make_shared<InspectorCamera>();
	if (FAILED(instance->Initialize())) return nullptr;
	return instance;
}
