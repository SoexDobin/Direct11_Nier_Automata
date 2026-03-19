#include "pch.h"
#include "LogLevel.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
	registration::class_<LogLevel>("LogLevel")
		.constructor<>()
		;
}
