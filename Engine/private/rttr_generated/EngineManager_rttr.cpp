#include "EngineManager.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

#line 3516
RTTR_REGISTRATION
{
    registration::class_<EngineManager>("EngineManager")
        .constructor<>()
        ;
}

