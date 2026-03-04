#include "InputDevice.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

#line 2685
RTTR_REGISTRATION
{
    registration::class_<InputDevice>("InputDevice")
        .constructor<>()
        .method("Create", &InputDevice::Create)
        ;
}

