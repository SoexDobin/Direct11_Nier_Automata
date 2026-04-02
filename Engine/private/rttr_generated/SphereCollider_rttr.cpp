#include "SphereCollider.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(SphereCollider_RTTR)
{
	registration::class_<SphereCollider>("SphereCollider")
		.constructor<>()
		.method("Clone", &SphereCollider::Clone)
		.method("Create", &SphereCollider::Create)
		.method("CreatePrototype", &SphereCollider::CreatePrototype)
		;
}
