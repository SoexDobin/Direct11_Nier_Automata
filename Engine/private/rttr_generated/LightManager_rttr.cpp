#include "LightManager.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<LightManager>(L"LightManager")
        .constructor<>()
        .method("Create", &LightManager::Create)
        ;
}

