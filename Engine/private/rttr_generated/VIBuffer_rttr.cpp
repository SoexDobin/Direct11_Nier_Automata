#include "VIBuffer.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(VIBuffer_RTTR)
{
    registration::class_<VIBuffer>("VIBuffer")
        ;
}

