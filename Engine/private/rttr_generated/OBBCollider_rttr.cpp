#include "OBBCollider.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(OBBCollider_RTTR)
{
	registration::class_<OBBCollider>("OBBCollider")
		.constructor<>()
		.method("Clone", &OBBCollider::Clone)
		.method("Create", &OBBCollider::Create)
		.method("CreatePrototype", &OBBCollider::CreatePrototype)
		;
}
