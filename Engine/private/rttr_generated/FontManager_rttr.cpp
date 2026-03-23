#include "pch.h"
#include "FontManager.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
	registration::class_<FontManager>("FontManager")
		.constructor<>()
		;
}
