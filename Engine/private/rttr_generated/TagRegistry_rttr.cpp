#include "TagRegistry.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(TagRegistry_RTTR)
{
	registration::class_<TagRegistry>("TagRegistry")
		.constructor<>()
		;
}
