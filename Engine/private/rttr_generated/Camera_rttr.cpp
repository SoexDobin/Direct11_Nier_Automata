#include "Camera.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

using namespace Meta_Key_Type;
using namespace Data_Type_Key;
using namespace Data_Tag;
using namespace Asset_Type_Key;
using namespace Save_Data_Key;

RTTR_REGISTRATION_NAMED(Camera_RTTR)
{
	registration::class_<Camera>("Camera")
		.method("Clone", &Camera::Clone)
		.property("FovY", &Camera::Get_FovY, &Camera::Set_FovY)
		.property("Aspect", &Camera::Get_Aspect, &Camera::Set_Aspect)
		.property("Near", &Camera::Get_NearPlane, &Camera::Set_NearPlane)
		.property("Far", &Camera::Get_FarPlane, &Camera::Set_FarPlane)
		.property("TargetID", &Camera::Get_TargetID, &Camera::Set_TargetID)
		(
			metadata(DataType, Data_Type_Key::uint32),
			metadata(TypeTag, Data_Tag::ObjectID),
			metadata(SaveData, Save_Data_Key::TargetObjectID)
		);
}
