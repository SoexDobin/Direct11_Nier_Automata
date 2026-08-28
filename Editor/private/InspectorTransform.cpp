#include "pch.h"
#include "InspectorTransform.h"

#include <PartObject.h>

#include "EditorManager.h"
#include "Transform.h"

using namespace Editor;
using namespace Engine;

HRESULT InspectorTransform::Initialize()
{
    return S_OK;
}

void InspectorTransform::RenderComponent(const Shared<Transform>& transform)
{
	if (!transform) return;

    if (ImGui::CollapsingHeader("Transform Component", ImGuiTreeNodeFlags_DefaultOpen))
    {
        Vector3 pos = transform->Get_LocalPosition();
        Vector3 rot = transform->Get_LocalEulerAngles();
        Vector3 scale = transform->Get_LocalScale();

        ImGui::Text("Position");
        ImGui::PushItemWidth(-1);
        if (ImGui::DragFloat3("##Pos", reinterpret_cast<Float*>(&pos), 0.1f)) {
            transform->Set_LocalPositionByValue(pos);
            CheckPart(transform->Get_Owner());
        }
        ImGui::PopItemWidth();

        ImGui::Text("Rotation (Degrees)");
        ImGui::PushItemWidth(-1);
        if (ImGui::DragFloat3("##Rot", reinterpret_cast<Float*>(&rot), 1.0f, 0.0f, 360.0f)) {
            transform->Set_LocalEulerAngleByValue(rot);
            CheckPart(transform->Get_Owner());
        }
        ImGui::PopItemWidth();

        ImGui::Text("Scale");
        ImGui::PushItemWidth(-1);
        if (ImGui::DragFloat3("##Scale", reinterpret_cast<Float*>(&scale), 0.05f, 0.001f, 100.0f)) {
            transform->Set_LocalScaleByValue(scale);
            CheckPart(transform->Get_Owner());
        }
        ImGui::PopItemWidth();

        ImGui::Spacing();
    }
}

void InspectorTransform::CheckPart(const Shared<GameObject>& isPart)
{
    auto owner = isPart;
	if (!owner)
		return;

	for (const auto& child : owner->Get_Children())
    {
		const auto part = dynamic_pointer_cast<PartObject>(child);
		if (!part)
			continue;
		part->Get_Transform()->Update_WorldMatrix();
		part->Update(0.f);
		part->Late_Update(0.f);
    }
}

Shared<InspectorTransform> InspectorTransform::Create()
{
	auto instance = make_shared<InspectorTransform>();
	if (FAILED(instance->Initialize())) return nullptr;
	return instance;
}
