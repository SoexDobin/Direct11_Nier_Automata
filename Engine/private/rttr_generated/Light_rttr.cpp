#include "Light.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Light_RTTR)
{
	registration::class_<Light>("Light")
		.constructor<>()
		.method("Create", &Light::Create)
		;
}
