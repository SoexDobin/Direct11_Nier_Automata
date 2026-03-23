#include "LayerRegistry.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(LayerRegistry_RTTR)
{
	registration::class_<LayerRegistry>("LayerRegistry")
		.constructor<>()
		;
}
