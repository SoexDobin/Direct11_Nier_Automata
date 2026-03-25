#include "EngineManager.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(EngineManager_RTTR)
{
	registration::class_<EngineManager>("EngineManager")
		;
}
