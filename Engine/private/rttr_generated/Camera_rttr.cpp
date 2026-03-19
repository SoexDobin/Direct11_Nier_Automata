#include "Camera.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Camera_RTTR)
{
	registration::class_<Camera>("Camera")
		.property("GameObject", &Camera::Get_TargetID, &Camera::Set_TargetID)
			(metadata("SaveData", "GameObject"))
		.property("FovY", &Camera::Get_FovY, &Camera::Set_FovY)
			(metadata("SaveData", "SliderFloat"), metadata("Min", 0.1f), metadata("Max", 3.14f))
		.property("Aspect", &Camera::Get_Aspect, &Camera::Set_Aspect)
			(metadata("SaveData", true))
		.property("Near", &Camera::Get_NearPlane, &Camera::Set_NearPlane)
			(metadata("SaveData", true))
		.property("Far", &Camera::Get_FarPlane, &Camera::Set_FarPlane)
			(metadata("SaveData", true))
		.method("Clone", &Camera::Clone)
		;
}
