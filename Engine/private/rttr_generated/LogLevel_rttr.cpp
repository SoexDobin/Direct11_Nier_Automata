#include "LogLevel.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(LogLevel_RTTR)
{
	registration::class_<LogLevel>("LogLevel")
		.constructor<>()
		;
}
