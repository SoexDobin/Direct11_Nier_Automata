#include "Animation.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Animation_RTTR)
{
	registration::class_<Animation>("Animation")
		.constructor<>()
		.method("Clone", &Animation::Clone)
		.method("Create", &Animation::Create)
		;
}
