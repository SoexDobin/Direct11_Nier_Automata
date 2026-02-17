#include "VIBuffer_Rect.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<VIBuffer_Rect>(L"VIBuffer_Rect")
        .constructor<>()
        .method("Clone", &VIBuffer_Rect::Clone)
        ;
}

