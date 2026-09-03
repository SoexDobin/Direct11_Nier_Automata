#include "Object.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Object_RTTR)
{
	registration::class_<Object>("Object")
		;
}
