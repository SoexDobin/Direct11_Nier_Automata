#include "LightManager.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(LightManager_RTTR)
{
	registration::class_<LightManager>("LightManager")
		.constructor<>()
		.method("Create", &LightManager::Create)
		;
}
