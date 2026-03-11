#include "pch.h"
#include "LayerRegistry.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

#line 5936
RTTR_REGISTRATION
{
	registration::class_<LayerRegistry>("LayerRegistry")
		.constructor<>()
		;
}
