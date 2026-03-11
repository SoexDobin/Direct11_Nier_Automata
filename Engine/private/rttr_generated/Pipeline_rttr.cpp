#include "Pipeline.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

#line 4925
RTTR_REGISTRATION
{
    registration::class_<Pipeline>("Pipeline")
        .constructor<>()
        .method("Create", &Pipeline::Create)
        ;
}

