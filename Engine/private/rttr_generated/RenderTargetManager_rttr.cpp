#include "RenderTargetManager.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(RenderTargetManager_RTTR)
{
	registration::class_<RenderTargetManager>("RenderTargetManager")
		.constructor<>()
		.method("Create", &RenderTargetManager::Create)
		;
}
