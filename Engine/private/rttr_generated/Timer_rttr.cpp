#include "Timer.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Timer_RTTR)
{
	registration::class_<Timer>("Timer")
		.constructor<>()
		.method("Create", &Timer::Create)
		;
}
