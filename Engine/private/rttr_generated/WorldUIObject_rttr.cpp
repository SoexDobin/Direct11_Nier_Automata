#include "WorldUIObject.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(WorldUIObject_RTTR)
{
	registration::class_<WorldUIObject>("WorldUIObject")
		;
}
