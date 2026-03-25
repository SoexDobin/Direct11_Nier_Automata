#include "pch.h"
#include "InspectorTransform.h"
#include "EditorManager.h"
#include "Transform.h"

using namespace Editor;
using namespace Engine;

HRESULT InspectorTransform::Initialize()
{
    return S_OK;
}

void InspectorTransform::RenderComponent(const Shared<Transform>& pTransform)
{
	if (!pTransform) return;

    if (ImGui::CollapsingHeader("Transform Component", ImGuiTreeNodeFlags_DefaultOpen))
    {
        Vector3 pos = pTransform->Get_LocalPosition();
        Vector3 rot = pTransform->Get_LocalEulerAngles();
        Vector3 scale = pTransform->Get_LocalScale();

        ImGui::Text("Position");
        ImGui::PushItemWidth(-1);
        if (ImGui::DragFloat3("##Pos", reinterpret_cast<float*>(&pos), 0.1f)) {
            pTransform->Set_LocalPositionByValue(pos);
        }
        ImGui::PopItemWidth();

        ImGui::Text("Rotation (Degrees)");
        ImGui::PushItemWidth(-1);
        if (ImGui::DragFloat3("##Rot", reinterpret_cast<float*>(&rot), 1.0f, 0.0f, 360.0f)) {
            pTransform->Set_LocalEulerAngleByValue(rot);
        }
        ImGui::PopItemWidth();

        ImGui::Text("Scale");
        ImGui::PushItemWidth(-1);
        if (ImGui::DragFloat3("##Scale", reinterpret_cast<float*>(&scale), 0.05f, 0.001f, 100.0f)) {
            pTransform->Set_LocalScaleByValue(scale);
        }
        ImGui::PopItemWidth();

        ImGui::Spacing();
    }
}

Shared<InspectorTransform> InspectorTransform::Create()
{
	auto instance = make_shared<InspectorTransform>();
	if (FAILED(instance->Initialize())) return nullptr;
	return instance;
}
