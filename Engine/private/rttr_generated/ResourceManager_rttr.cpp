#include "pch.h"
#include "ResourceManager.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
	registration::class_<ResourceManager>("ResourceManager")
		.constructor<>()
		.method("Create", &ResourceManager::Create)
		;
}
