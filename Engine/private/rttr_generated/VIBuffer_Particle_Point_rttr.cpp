#include "VIBuffer_Particle_Point.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(VIBuffer_Particle_Point_RTTR)
{
	registration::class_<VIBuffer_Particle_Point>("VIBuffer_Particle_Point")
		.constructor<>()
		.method("Clone", &VIBuffer_Particle_Point::Clone)
		.method("CreatePrototype", &VIBuffer_Particle_Point::CreatePrototype)
		;
}
