#include "VIBuffer_Instance.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(VIBuffer_Instance_RTTR)
{
	registration::class_<VIBuffer_Instance>("VIBuffer_Instance")
		.method("Clone", &VIBuffer_Instance::Clone)
		;
}
