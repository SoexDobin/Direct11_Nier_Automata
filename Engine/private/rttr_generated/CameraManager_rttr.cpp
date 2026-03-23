#include "CameraManager.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(CameraManager_RTTR)
{
	registration::class_<CameraManager>("CameraManager")
		.constructor<>()
		.method("Create", &CameraManager::Create)
		;
}
