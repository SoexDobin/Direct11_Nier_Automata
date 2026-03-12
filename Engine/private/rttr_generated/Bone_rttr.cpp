#include "pch.h"
#include "Bone.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
	registration::class_<Bone>("Bone")
		.constructor<>()
		.method("Clone", &Bone::Clone)
		;
}
