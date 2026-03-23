#include "Level.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Level_RTTR)
{
	registration::class_<Level>("Level")
		;
}
