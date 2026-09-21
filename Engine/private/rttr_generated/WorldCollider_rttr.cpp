#include "WorldCollider.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(WorldCollider_RTTR)
{
	registration::class_<WorldCollider>("WorldCollider")
		.constructor<>()
		.method("Clone", &WorldCollider::Clone)
		.method("Create", &WorldCollider::Create)
		.method("CreatePrototype", &WorldCollider::CreatePrototype)
		;
}
