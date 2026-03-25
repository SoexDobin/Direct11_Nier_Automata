#include "TimeManager.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(TimeManager_RTTR)
{
	registration::class_<TimeManager>("TimeManager")
		.constructor<>()
		.method("Create", &TimeManager::Create)
		;
}
