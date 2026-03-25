#include "Renderer.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Renderer_RTTR)
{
	registration::class_<Renderer>("Renderer")
		.constructor<>()
		.method("Create", &Renderer::Create)
		;
}
