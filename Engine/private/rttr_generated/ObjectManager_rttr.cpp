#include "ObjectManager.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(ObjectManager_RTTR)
{
	registration::class_<ObjectManager>("ObjectManager")
		.constructor<>()
		.method("Create", &ObjectManager::Create)
		;
}
