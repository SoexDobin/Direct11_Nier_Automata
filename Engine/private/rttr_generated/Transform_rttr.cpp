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

	.property("Position", &Transform::Get_LocalPosition, &Transform::Set_LocalPositionByValue)
	(
		metadata(Meta_Key::DataType, "Vector3"),
		metadata(Meta_Key::Widget, Widget_Type::DragFloat3),
		metadata(Meta_Key::SaveData, Serialize_Data_Field::Position)
	)
	.property("Rotation", &Transform::Get_LocalEulerAngles, &Transform::Set_LocalEulerAngleByValue)
		(
			metadata(Meta_Key::DataType, "Vector3"),
			metadata(Meta_Key::Widget, Widget_Type::DragFloat3),
			metadata(Meta_Key::SaveData, Serialize_Data_Field::Rotation)
		)
	.property("Scale", &Transform::Get_LocalScale, &Transform::Set_LocalScaleByValue)
	(
		metadata(Meta_Key::DataType, "Vector3"),
		metadata(Meta_Key::Widget, Widget_Type::DragFloat3),
		metadata(Meta_Key::SaveData, Serialize_Data_Field::Scale)
	)
		;
}
