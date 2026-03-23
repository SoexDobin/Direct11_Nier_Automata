#include "FontManager.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(FontManager_RTTR)
{
	registration::class_<FontManager>("FontManager")
		.constructor<>()
		.method("Create", &FontManager::Create)
		;
}
