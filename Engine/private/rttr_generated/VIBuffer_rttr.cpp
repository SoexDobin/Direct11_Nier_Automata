#include "VIBuffer.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

#line 7202
RTTR_REGISTRATION
{
    registration::class_<VIBuffer>("VIBuffer")
        .constructor<>()
        .method("Clone", &VIBuffer::Clone)
        ;
}

