#include "pch.h"
#include "InspectorCollider.h"
#include "OBBCollider.h"
#include "SphereCollider.h"

using namespace Engine;
using namespace Editor;

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
			RenderOBB(static_pointer_cast<OBBCollider>(pCollider));
		else if (type == COMPONENT_TYPE::SPHERE_COLLIDER)
			RenderSphere(static_pointer_cast<SphereCollider>(pCollider));
	}

	ImGui::PopID();
}

void InspectorCollider::RenderOBB(const std::shared_ptr<Engine::OBBCollider>& pOBB)
{
	ImGui::TextDisabled("Type: OBB");
	ImGui::Separator();

	Vector3 offset = pOBB->Get_Offset();
	if (ImGui::DragFloat3("Offset", (float*)&offset, 0.05f)) {
		pOBB->Set_Offset(offset);
	}

	Vector3 extents = pOBB->Get_Extents();
	if (ImGui::DragFloat3("Extents(Half-Size)", (float*)&extents, 0.05f, 0.01f, 1000.f)) {
		pOBB->Set_Extents(extents);
	}
}

void InspectorCollider::RenderSphere(const std::shared_ptr<Engine::SphereCollider>& pSphere)
{
	ImGui::TextDisabled("Type: Sphere");
	ImGui::Separator();

	Vector3 offset = pSphere->Get_Offset();
	if (ImGui::DragFloat3("Offset", (float*)&offset, 0.05f)) {
		pSphere->Set_Offset(offset);
	}

	Float radius = pSphere->Get_Radius();
	if (ImGui::DragFloat("Radius", &radius, 0.05f, 0.01f, 1000.f)) {
		pSphere->Set_Radius(radius);
	}
}

std::shared_ptr<InspectorCollider> InspectorCollider::Create()
{
	auto instance = make_shared<InspectorCollider>();
	if (FAILED(instance->Initialize())) return nullptr;
	return instance;
}
