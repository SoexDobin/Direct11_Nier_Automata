#include "JsonSerializer.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<JsonSerializer>(L"JsonSerializer")
        .constructor<>()
        ;
}

