#include "Camera.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Camera_RTTR)
{
	registration::class_<Camera>("Camera")
		.property("TargetID", &Camera::Get_TargetID, &Camera::Set_TargetID)
		(
			metadata(Meta_Key::DataType, "uint32"),
			metadata(Meta_Key::Widget, "GameObject"),
			metadata(Meta_Key::SaveData, Serialize_Data_Field::GameObject)
		)
		.property("FovY", &Camera::Get_FovY, &Camera::Set_FovY)
		(
			metadata(Meta_Key::DataType, "float"),
			metadata(Meta_Key::Widget, Widget_Type::SliderFloat),
			metadata(Meta_Key::Min, 0.1f),
			metadata(Meta_Key::Max, 3.14f),
			metadata(Meta_Key::SaveData, "float")
		)
		.property("Aspect", &Camera::Get_Aspect, &Camera::Set_Aspect)
			(metadata("SaveData", true))
		.property("Near", &Camera::Get_NearPlane, &Camera::Set_NearPlane)
			(metadata("SaveData", true))
		.property("Far", &Camera::Get_FarPlane, &Camera::Set_FarPlane)
			(metadata("SaveData", true))
		.method("Clone", &Camera::Clone)
		;
}
