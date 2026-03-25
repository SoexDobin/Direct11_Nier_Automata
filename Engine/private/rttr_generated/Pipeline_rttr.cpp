#include "Pipeline.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Pipeline_RTTR)
{
	registration::class_<Pipeline>("Pipeline")
		.constructor<>()
		.method("Create", &Pipeline::Create)
		;
}
