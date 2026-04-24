#include "Shadow.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Shadow_RTTR)
{
	registration::class_<Shadow>("Shadow")
		.constructor<>()
		.method("Create", &Shadow::Create)
		;
}
