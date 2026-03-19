#include "pch.h"
#include "GraphicDevice.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
	registration::class_<GraphicDevice>("GraphicDevice")
		.constructor<>()
		.method("Create", &GraphicDevice::Create)
		;
}
