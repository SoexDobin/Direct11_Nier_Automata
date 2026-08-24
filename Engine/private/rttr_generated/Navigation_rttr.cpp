#include "Navigation.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Navigation_RTTR)
{
	registration::class_<Navigation>("Navigation")
		.constructor<>()
		.method("Clone", &Navigation::Clone)
		.method("Create", &Navigation::Create)
		.method("CreatePrototype", &Navigation::CreatePrototype)
		;
}
