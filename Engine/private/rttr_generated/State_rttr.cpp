#include "pch.h"
#include "State.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
	registration::class_<State>("State")
		.constructor<>()
		;
}
