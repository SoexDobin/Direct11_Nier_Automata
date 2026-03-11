#include "Timer.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

#line 4561
RTTR_REGISTRATION
{
    registration::class_<Timer>("Timer")
        .constructor<>()
        .method("Create", &Timer::Create)
        ;
}

