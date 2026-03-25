#include "PrototypeManager.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(PrototypeManager_RTTR)
{
	registration::class_<PrototypeManager>("PrototypeManager")
		.constructor<>()
		.method("Create", &PrototypeManager::Create)
		;
}
