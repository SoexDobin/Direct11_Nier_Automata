#include "Transform.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Transform_RTTR)
{
	registration::class_<Transform>("Transform")
	.constructor<>()

	.method("Clone", &Transform::Clone)
	.method("Create", &Transform::Create)
	.method("CreatePrototype", &Transform::CreatePrototype)

	.property("Position", &Transform::Get_LocalPosition,
		static_cast<void(Engine::Transform::*)(DirectX::SimpleMath::Vector3)>(&Transform::Set_LocalPositionByValue))(
			metadata("DataType", "Vector3"),
			metadata("Widget", "DragFloat3"),
			metadata("SaveVector3", &Transform::Get_LocalPosition)
		)
	.property("Rotation", &Transform::Get_LocalEulerAngles,
		static_cast<void(Engine::Transform::*)(DirectX::SimpleMath::Vector3)>(&Transform::Set_LocalEulerAngleByValue))(
			metadata("DataType", "Vector3"),
			metadata("Widget", "DragFloat3"),
			metadata("SaveVector3", &Transform::Get_LocalEulerAngles)
		)
	.property("Scale", &Transform::Get_LocalScale, 
		static_cast<void(Engine::Transform::*)(DirectX::SimpleMath::Vector3)>(&Transform::Set_LocalScaleByValue))(
			metadata("DataType", "Vector3"),
			metadata("Widget", "DragFloat3"),
			metadata("SaveVector3", &Transform::Get_LocalEulerAngles)
		)
		;
}
