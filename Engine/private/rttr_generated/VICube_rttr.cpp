#include "VICube.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(VICube_RTTR)
{
	registration::class_<VICube>("VICube")
		.constructor<>()
		.method("Clone", &VICube::Clone)
		.method("Create", &VICube::Create)
		.method("CreatePrototype", &VICube::CreatePrototype)
		;
}
