#include "OBBCollider.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

using namespace Meta_Key_Type;
using namespace Data_Type_Key;
using namespace Data_Tag;

RTTR_REGISTRATION_NAMED(OBBCollider_RTTR)
{
	registration::class_<OBBCollider>("OBBCollider")
		.constructor<>()
		.method("Clone", &OBBCollider::Clone)
		.method("Create", &OBBCollider::Create)
		.method("CreatePrototype", &OBBCollider::CreatePrototype)
		.property("Offset", &OBBCollider::Get_ReflectedOffset, &OBBCollider::Set_ReflectedOffset)
		(
			metadata(DataType, Data_Type_Key::Vector3),
			metadata(TypeTag, Data_Tag::NoneTag),
			metadata(SaveData, "Offset")
		)
		.property("Extents", &OBBCollider::Get_ReflectedExtents, &OBBCollider::Set_ReflectedExtents)
		(
			metadata(DataType, Data_Type_Key::Vector3),
			metadata(TypeTag, Data_Tag::NoneTag),
			metadata(SaveData, "Extents")
		)
		;
}
