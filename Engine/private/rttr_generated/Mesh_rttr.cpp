#include "Mesh.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Mesh_RTTR)
{
	registration::class_<Mesh>("Mesh")
		.constructor<>()
		.method("Clone", &Mesh::Clone)
		.method("Create", &Mesh::Create)
		.method("CreatePrototype", &Mesh::CreatePrototype)
		;
}
