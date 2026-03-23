#include "Material.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Material_RTTR)
{
	registration::class_<Material>("Material")
		.constructor<>()
		.method("Clone", &Material::Clone)
		.method("Create", &Material::Create)
		.method("CreatePrototype", &Material::CreatePrototype)
		;
}
