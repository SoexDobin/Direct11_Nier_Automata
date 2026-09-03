#include "pch.h"
#include "InspectorCollider.h"
#include "EditorManager.h"
#include "Component.h"
#include "Game.h"

using namespace Engine;
using namespace Editor;

namespace
{
	template <typename T>
	Bool ReadColliderValue(Object& object, std::string_view propertyName, T& outValue)
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
	void QueueColliderValue(Component& component, std::string_view propertyName,
		const T& before, const T& after)
	{
		ReflectionValue beforeValue;
		ReflectionValue afterValue;
		beforeValue.data = before;
		afterValue.data = after;
		EDITOR->Queue_PropertyWrite(component.Get_Owner(), component, propertyName,
			beforeValue, afterValue,
			ImGui::IsItemActivated() || !ImGui::IsItemActive());
	}
}

HRESULT InspectorCollider::Initialize()
{
	return EditorObject::Initialize();
}

void InspectorCollider::RenderComponent(const std::shared_ptr<Engine::Component>& pCollider)
{
	if (!pCollider) return;

	auto type = pCollider->Get_ComponentType();
	string headerTitle = "Collider##";

	if (type == COMPONENT_TYPE::OBB_COLLIDER) headerTitle = "OBB Collider##";
	else if (type == COMPONENT_TYPE::SPHERE_COLLIDER) headerTitle = "Sphere Collider##";

	// 다중 콜라이더의 ImGui ID 충돌을 방지하기 위해 메모리 주소를 ID로 사용
	headerTitle += std::to_string(reinterpret_cast<uint64_t>(pCollider.get()));

	ImGui::PushID(pCollider.get());

	if (ImGui::CollapsingHeader(headerTitle.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (type == COMPONENT_TYPE::OBB_COLLIDER)
			RenderOBB(*pCollider);
		else if (type == COMPONENT_TYPE::SPHERE_COLLIDER)
			RenderSphere(*pCollider);
	}

	ImGui::PopID();
}

void InspectorCollider::RenderOBB(Component& collider)
{
	ImGui::TextDisabled("Type: OBB");
	ImGui::Separator();

	Vector3 offset{};
	if (ReadColliderValue(collider, "Offset", offset)) {
		const Vector3 beforeOffset = offset;
		if (ImGui::DragFloat3("Offset", reinterpret_cast<Float*>(&offset), 0.05f))
			QueueColliderValue(collider, "Offset", beforeOffset, offset);
	}

	Vector3 extents{};
	if (ReadColliderValue(collider, "Extents", extents)) {
		const Vector3 beforeExtents = extents;
		if (ImGui::DragFloat3("Extents(Half-Size)", reinterpret_cast<Float*>(&extents),
			0.05f, 0.01f, 1000.f))
			QueueColliderValue(collider, "Extents", beforeExtents, extents);
	}
}

void InspectorCollider::RenderSphere(Component& collider)
{
	ImGui::TextDisabled("Type: Sphere");
	ImGui::Separator();

	Vector3 offset{};
	if (ReadColliderValue(collider, "Offset", offset)) {
		const Vector3 beforeOffset = offset;
		if (ImGui::DragFloat3("Offset", reinterpret_cast<Float*>(&offset), 0.05f))
			QueueColliderValue(collider, "Offset", beforeOffset, offset);
	}

	Float radius{};
	if (ReadColliderValue(collider, "Radius", radius)) {
		const Float beforeRadius = radius;
		if (ImGui::DragFloat("Radius", &radius, 0.05f, 0.01f, 1000.f))
			QueueColliderValue(collider, "Radius", beforeRadius, radius);
	}
}

std::shared_ptr<InspectorCollider> InspectorCollider::Create()
{
	auto instance = make_shared<InspectorCollider>();
	if (FAILED(instance->Initialize())) return nullptr;
	return instance;
}
