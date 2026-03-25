#include "Bone.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Bone_RTTR)
{
	registration::class_<Bone>("Bone")
		.constructor<>()
		.method("Clone", &Bone::Clone)
		.method("Create", &Bone::Create)
		;
}
