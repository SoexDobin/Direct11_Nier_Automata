#include "RenderTarget.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(RenderTarget_RTTR)
{
	registration::class_<RenderTarget>("RenderTarget")
		.constructor<>()
		.method("Create", &RenderTarget::Create)
		;
}
