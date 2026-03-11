#include "LightManager.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

#line 4981
RTTR_REGISTRATION
{
    registration::class_<LightManager>("LightManager")
        .constructor<>()
        .method("Create", &LightManager::Create)
        ;
}

