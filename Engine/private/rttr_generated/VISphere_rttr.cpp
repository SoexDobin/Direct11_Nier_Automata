#include "VISphere.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(VISphere_RTTR)
{
	registration::class_<VISphere>("VISphere")
		.constructor<>()
		.method("Clone", &VISphere::Clone)
		.method("Create", &VISphere::Create)
		.method("CreatePrototype", &VISphere::CreatePrototype)
		;
}
