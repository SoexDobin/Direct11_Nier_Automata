#include "GameObject.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION {
	registration::class_<GameObject>("GameObject")
		.property_readonly("Layer", &GameObject::Get_LayerMask)
		.property_readonly("Tag", &GameObject::Get_TagMask)
		;
}
