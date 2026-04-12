#include "VIBuffer_Particle_Rect.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(VIBuffer_Particle_Rect_RTTR)
{
	registration::class_<VIBuffer_Particle_Rect>("VIBuffer_Particle_Rect")
		.constructor<>()
		.method("Clone", &VIBuffer_Particle_Rect::Clone)
		.method("CreatePrototype", &VIBuffer_Particle_Rect::CreatePrototype)
		;
}
