#include "EngineManager.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<EngineManager>(L"EngineManager")
        .constructor<>()
        ;
}

