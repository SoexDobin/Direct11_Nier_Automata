#include "pch.h"
#include "InspectorCamera.h"
#include "Editor_Define.h"
#include "EditorManager.h"
#include "Camera.h"
#include "Game.h"
#include "GameObject.h"
#include "String_Helper.h"
#include "SpdLogger.h"

using namespace Editor;
using namespace Engine;

HRESULT InspectorCamera::Initialize()
{
    return S_OK;
}

void InspectorCamera::RenderCamera(const Shared<GameObject>& pObj)
{
	auto pCamera = static_pointer_cast<Camera>(pObj);
	if (!pCamera) return;

    if (ImGui::CollapsingHeader("Camera Component", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // 1. FovY
        float fovY = pCamera->Get_FovY();
        if (ImGui::SliderAngle("FovY", &fovY, 1.0f, 179.0f)) {
            pCamera->Set_FovY(fovY);
        }

        // 2. Aspect
        float aspect = pCamera->Get_Aspect();
        if (ImGui::DragFloat("Aspect", &aspect, 0.01f, 0.1f, 10.0f)) {
            pCamera->Set_Aspect(aspect);
        }

        // 3. Near / Far
        float nearPlane = pCamera->Get_NearPlane();
        if (ImGui::DragFloat("Near", &nearPlane, 0.01f, 0.001f, 1000.0f)) {
            pCamera->Set_NearPlane(nearPlane);
        }

        float farPlane = pCamera->Get_FarPlane();
        if (ImGui::DragFloat("Far", &farPlane, 1.0f, 1.0f, 10000.0f)) {
            pCamera->Set_FarPlane(farPlane);
        }
        
        // 4. TargetID (GameObject Drag & Drop)
        uint32 currentTargetID = pCamera->Get_TargetID();
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
                    pCamera->Set_TargetID(droppedObj->Get_ObjectID());
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        if (ImGui::Button("X##TargetID", ImVec2(30, 25))) {
            pCamera->Set_TargetID(0u);
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
