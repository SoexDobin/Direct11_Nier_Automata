#include "pch.h"
#include "WINMODE.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
	registration::class_<WINMODE>("WINMODE")
		.constructor<>()
		;
}
