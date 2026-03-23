#include "pch.h"
#include "SoundManager.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
	registration::class_<SoundManager>("SoundManager")
		.constructor<>()
		.method("Create", &SoundManager::Create)
		;
}
