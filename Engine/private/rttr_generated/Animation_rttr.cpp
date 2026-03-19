#include "pch.h"
#include "Animation.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
	registration::class_<Animation>("Animation")
		.constructor<>()
		.parent<Component>()
		.method("Clone", &Animation::Clone)
		.method("Create", &Animation::Create)
		;
}
