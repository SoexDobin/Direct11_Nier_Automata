#include "pch.h"
#include "InspectorTransform.h"

#include <PartObject.h>

#include "EditorManager.h"
#include "Game.h"
#include "Transform.h"

using namespace Editor;
using namespace Engine;

namespace
{
	Bool ReadVector3(Object& object, std::string_view propertyName, Vector3& outValue)
	{
		ReflectionValue reflectedValue;
		if (FAILED(GAME_INSTANCE->Read_ReflectedProperty(object, propertyName, reflectedValue)))
			return false;
		const Vector3* value = reflectedValue.Try_Get<Vector3>();
		if (!value)
			return false;
		outValue = *value;
		return true;
	}

	Bool WriteVector3(Object& object, std::string_view propertyName, const Vector3& value)
	{
		ReflectionValue reflectedValue;
		reflectedValue.data = value;
		return SUCCEEDED(GAME_INSTANCE->Write_ReflectedProperty(
			object, propertyName, reflectedValue));
	}
}

HRESULT InspectorTransform::Initialize()
{
    return S_OK;
}

void InspectorTransform::RenderComponent(const Shared<Transform>& transform)
{
	if (!transform) return;

    if (ImGui::CollapsingHeader("Transform Component", ImGuiTreeNodeFlags_DefaultOpen))
    {
		Vector3 pos{};
		Vector3 rot{};
		Vector3 scale{};
		if (!ReadVector3(*transform, "Position", pos) ||
			!ReadVector3(*transform, "Rotation", rot) ||
			!ReadVector3(*transform, "Scale", scale)) {
			ImGui::TextDisabled("Transform reflection properties are unavailable.");
			return;
		}

        ImGui::Text("Position");
        ImGui::PushItemWidth(-1);
        if (ImGui::DragFloat3("##Pos", reinterpret_cast<Float*>(&pos), 0.1f)) {
			if (WriteVector3(*transform, "Position", pos))
				CheckPart(transform->Get_Owner());
        }
        ImGui::PopItemWidth();

        ImGui::Text("Rotation (Degrees)");
        ImGui::PushItemWidth(-1);
        if (ImGui::DragFloat3("##Rot", reinterpret_cast<Float*>(&rot), 1.0f, 0.0f, 360.0f)) {
			if (WriteVector3(*transform, "Rotation", rot))
				CheckPart(transform->Get_Owner());
        }
        ImGui::PopItemWidth();

        ImGui::Text("Scale");
        ImGui::PushItemWidth(-1);
        if (ImGui::DragFloat3("##Scale", reinterpret_cast<Float*>(&scale), 0.05f, 0.001f, 100.0f)) {
			if (WriteVector3(*transform, "Scale", scale))
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
