#include "Component.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Component_RTTR)
{
	registration::class_<Component>("Component")
		.method("Clone", &Component::Clone)
		;
}
