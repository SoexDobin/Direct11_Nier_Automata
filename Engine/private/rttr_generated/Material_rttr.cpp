#include "pch.h"
#include "Material.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
	registration::class_<Material>("Material")
		.constructor<>()
		;
}
