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
		static_cast<void(Engine::Transform::*)(DirectX::SimpleMath::Vector3)>(&Transform::Set_LocalPositionByValue))
	.property("Rotation", &Transform::Get_LocalRotation,
		static_cast<void(Engine::Transform::*)(DirectX::SimpleMath::Quaternion)>(&Transform::Set_LocalRotationByValue))
	.property("Scale", &Transform::Get_LocalScale, 
		static_cast<void(Engine::Transform::*)(DirectX::SimpleMath::Vector3)>(&Transform::Set_LocalScaleByValue))
		;
}
