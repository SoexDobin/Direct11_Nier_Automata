#include "Collider.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Collider_RTTR)
{
	registration::class_<Collider>("Collider")
		.method("Clone", &Collider::Clone)
		;
}
