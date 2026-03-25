#include "InputDevice.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(InputDevice_RTTR)
{
	registration::class_<InputDevice>("InputDevice")
		.constructor<>()
		.method("Create", &InputDevice::Create)
		;
}
