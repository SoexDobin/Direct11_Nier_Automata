#include "VIBuffer.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<VIBuffer>(L"VIBuffer")
        .constructor<>()
        .method("Clone", &VIBuffer::Clone)
        ;
}

