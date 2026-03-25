#include "Font.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Font_RTTR)
{
	registration::class_<Font>("Font")
		.constructor<>()
		.method("Create", &Font::Create)
		;
}
