#include "ContainerObject.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(ContainerObject_RTTR)
{
	registration::class_<ContainerObject>("ContainerObject")
		.method("Clone", &ContainerObject::Clone)
		;
}
