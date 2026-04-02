#include "CollisionManager.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(CollisionManager_RTTR)
{
	registration::class_<CollisionManager>("CollisionManager")
		.constructor<>()
		;
}
