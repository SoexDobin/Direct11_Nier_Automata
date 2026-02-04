#include "LayerRegistry.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<LayerRegistry>(L"LayerRegistry")
        .constructor<>()
        ;
}

