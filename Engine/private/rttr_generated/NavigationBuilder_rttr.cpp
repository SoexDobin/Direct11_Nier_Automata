#include "NavigationBuilder.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(NavigationBuilder_RTTR)
{
	registration::class_<NavigationBuilder>("NavigationBuilder")
		.constructor<>()
		.method("Create", &NavigationBuilder::Create)
		;
}
