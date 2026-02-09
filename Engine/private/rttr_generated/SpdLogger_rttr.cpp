#include "SpdLogger.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<SpdLogger>(L"SpdLogger")
        .constructor<>()
        ;
}

