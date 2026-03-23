#include "VIBuffer_Rect.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(VIBuffer_Rect_RTTR)
{
	registration::class_<VIBuffer_Rect>("VIBuffer_Rect")
		.constructor<>()
		.method("Clone", &VIBuffer_Rect::Clone)
		.method("Create", &VIBuffer_Rect::Create)
		.method("CreatePrototype", &VIBuffer_Rect::CreatePrototype)
		;
}
