#include "SphereCollider.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

using namespace Meta_Key_Type;
using namespace Data_Type_Key;
using namespace Data_Tag;

RTTR_REGISTRATION_NAMED(SphereCollider_RTTR)
{
	registration::class_<SphereCollider>("SphereCollider")
		.constructor<>()
		.method("Clone", &SphereCollider::Clone)
		.method("Create", &SphereCollider::Create)
		.method("CreatePrototype", &SphereCollider::CreatePrototype)
		.property("Offset", &SphereCollider::Get_ReflectedOffset, &SphereCollider::Set_ReflectedOffset)
		(
			metadata(DataType, Data_Type_Key::Vector3),
			metadata(TypeTag, Data_Tag::NoneTag),
			metadata(SaveData, "Offset")
		)
		.property("Radius", &SphereCollider::Get_Radius, &SphereCollider::Set_Radius)
		(
			metadata(DataType, Data_Type_Key::Float),
			metadata(TypeTag, Data_Tag::NoneTag),
			metadata(SaveData, "Radius")
		)
		;
}
