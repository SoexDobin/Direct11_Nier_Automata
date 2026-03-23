#include "TextUI.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(TextUI_RTTR)
{
	registration::class_<TextUI>("TextUI")
		.constructor<>()
		.method("Clone", &TextUI::Clone)
		.method("Create", &TextUI::Create)
		.method("CreatePrototype", &TextUI::CreatePrototype)
		;
}
