#include "SoundManager.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(SoundManager_RTTR)
{
	registration::class_<SoundManager>("SoundManager")
		.constructor<>()
		.method("Create", &SoundManager::Create)
		;
}
