#include "LightGameObject.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(LightGameObject_RTTR)
{
	registration::class_<LightGameObject>("LightGameObject")
		.method("Clone", &LightGameObject::Clone)
		;
}
