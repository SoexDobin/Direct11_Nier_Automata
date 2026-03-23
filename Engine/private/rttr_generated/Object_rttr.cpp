#include "Object.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Object_RTTR)
{
	registration::class_<Object>("Object")
		.property_readonly("ObjectID", &Object::Get_ObjectID)
		(rttr::metadata(Meta_Key_Type::DataType, Data_Type_Key::uint32),
		rttr::metadata(Meta_Key_Type::TypeTag, Data_Tag::ObjectID),
		rttr::metadata(Meta_Key_Type::SaveData, Save_Data_Key::MyObjectID));
}
