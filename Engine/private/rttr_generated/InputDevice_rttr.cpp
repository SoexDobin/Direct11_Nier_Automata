#include "InputDevice.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<InputDevice>(L"InputDevice")
        .constructor<>()
        .method("Create", &InputDevice::Create)
        ;
}

