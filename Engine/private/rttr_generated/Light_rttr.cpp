#include "Light.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

#line 7220
RTTR_REGISTRATION
{
    registration::class_<Light>("Light")
        .constructor<>()
        .method("Create", &Light::Create)
        ;
}

