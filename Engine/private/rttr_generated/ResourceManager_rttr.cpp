#include "ResourceManager.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(ResourceManager_RTTR)
{
	registration::class_<ResourceManager>("ResourceManager")
		.constructor<>()
		.method("Create", &ResourceManager::Create)
		;
}
