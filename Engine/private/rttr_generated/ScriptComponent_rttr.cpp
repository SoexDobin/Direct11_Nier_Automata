#include "ScriptComponent.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<ScriptComponent>(L"ScriptComponent")
        .constructor<>()
        ;
}

