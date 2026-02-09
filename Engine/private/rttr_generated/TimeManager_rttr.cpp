#include "TimeManager.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<TimeManager>(L"TimeManager")
        .constructor<>()
        .method("Create", &TimeManager::Create)
        ;
}

