#include "EventManager.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(EventManager_RTTR)
{
	registration::class_<EventManager>("EventManager")
		.constructor<>()
		.method("Create", &EventManager::Create)
		;
}
