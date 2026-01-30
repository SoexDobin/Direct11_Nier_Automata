#include "GraphicDevice.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;
RTTR_REGISTRATION
{
    registration::class_<GraphicDevice>(L"GraphicDevice")
        (
            rttr::metadata("parent", L"")
        )
        .constructor<>()
    ;
}
