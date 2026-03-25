#include "LevelManager.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(LevelManager_RTTR)
{
	registration::class_<LevelManager>("LevelManager")
		.constructor<>()
		.method("Create", &LevelManager::Create)
		;
}
