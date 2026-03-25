#include "UIObject.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(UIObject_RTTR)
{
	registration::class_<UIObject>("UIObject")
		.method("Clone", &UIObject::Clone)
		;
}
