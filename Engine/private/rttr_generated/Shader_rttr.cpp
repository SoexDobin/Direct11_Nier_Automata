#include "Shader.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

#line 1627
RTTR_REGISTRATION
{
    registration::class_<Shader>("Shader")
        .constructor<>()
        .method("Clone", &Shader::Clone)
        .method("Create", &Shader::Create)
        ;
}

