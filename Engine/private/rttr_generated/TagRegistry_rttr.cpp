#include "pch.h"
#include "TagRegistry.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
	registration::class_<TagRegistry>("TagRegistry")
		.constructor<>()
		;
}
