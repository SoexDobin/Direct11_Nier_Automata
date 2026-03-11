#include "TimeManager.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

#line 1513
RTTR_REGISTRATION
{
    registration::class_<TimeManager>("TimeManager")
        .constructor<>()
        .method("Create", &TimeManager::Create)
        ;
}

