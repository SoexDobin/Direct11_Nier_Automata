#include "AABBCollider.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(AABBCollider_RTTR)
{
	registration::class_<AABBCollider>("AABBCollider")
		.constructor<>()
		.method("Clone", &AABBCollider::Clone)
		.method("Create", &AABBCollider::Create)
		.method("CreatePrototype", &AABBCollider::CreatePrototype)
		;
}
