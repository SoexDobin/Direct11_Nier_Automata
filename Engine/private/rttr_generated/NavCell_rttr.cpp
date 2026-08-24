#include "NavCell.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(NavCell_RTTR)
{
	registration::class_<NavCell>("NavCell")
		.constructor<>()
		.method("Create", &NavCell::Create)
		;
}
