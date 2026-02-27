#include "Light.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<Light>(L"Light")
        .constructor<>()
        .method("Create", &Light::Create)
        ;
}

