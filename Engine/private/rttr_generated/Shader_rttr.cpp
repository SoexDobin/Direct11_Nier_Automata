#include "Shader.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<Shader>(L"Shader")
        .constructor<>()
        .method("Clone", &Shader::Clone)
        .method("Create", &Shader::Create)
        ;
}

