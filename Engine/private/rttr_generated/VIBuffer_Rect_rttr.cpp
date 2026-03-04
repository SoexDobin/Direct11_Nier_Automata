#include "VIBuffer_Rect.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

#line 2868
RTTR_REGISTRATION
{
    registration::class_<VIBuffer_Rect>("VIBuffer_Rect")
        .constructor<>()
        .method("Clone", &VIBuffer_Rect::Clone)
        ;
}

