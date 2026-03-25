#include "PartObject.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(PartObject_RTTR)
{
	registration::class_<PartObject>("PartObject")
		.method("Clone", &PartObject::Clone)
		;
}
