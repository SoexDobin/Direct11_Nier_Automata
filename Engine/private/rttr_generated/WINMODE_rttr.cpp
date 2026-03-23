#include "WINMODE.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(WINMODE_RTTR)
{
	registration::class_<WINMODE>("WINMODE")
		.constructor<>()
		;
}
