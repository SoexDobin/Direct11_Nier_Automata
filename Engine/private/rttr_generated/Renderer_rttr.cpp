#include "Renderer.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

#line 7365
RTTR_REGISTRATION
{
    registration::class_<Renderer>("Renderer")
        .constructor<>()
        .method("Create", &Renderer::Create)
        ;
}

