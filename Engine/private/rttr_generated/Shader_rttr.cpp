#include "Shader.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Shader_RTTR)
{
	registration::class_<Shader>("Shader")
		.constructor<>()
		.method("Clone", &Shader::Clone)
		.method("Create", &Shader::Create)
		.method("CreatePrototype", &Shader::CreatePrototype)
		;
}
