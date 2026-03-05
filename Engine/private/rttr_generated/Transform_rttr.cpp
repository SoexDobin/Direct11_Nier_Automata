#include "Transform.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION{
	registration::class_<Transform>("Transform")
	.constructor<>()

	.method("Clone", &Transform::Clone)
	.method("Create", &Transform::Create)

	.property("Position", &Transform::Get_LocalPosition,
		static_cast<void(Engine::Transform::*)(DirectX::SimpleMath::Vector3)>(&Transform::Set_LocalPositionByValue))(
			metadata("Widget", "DragFloat3")
		)
	.property("Rotation", &Transform::Get_LocalEulerAngles,
		static_cast<void(Engine::Transform::*)(DirectX::SimpleMath::Vector3)>(&Transform::Set_LocalEulerAngleByValue))(
			metadata("Widget", "DragFloat3")
		)
	.property("Scale", &Transform::Get_LocalScale, 
		static_cast<void(Engine::Transform::*)(DirectX::SimpleMath::Vector3)>(&Transform::Set_LocalScaleByValue))(
			metadata("Widget", "DragFloat3")
		)
		;
}
