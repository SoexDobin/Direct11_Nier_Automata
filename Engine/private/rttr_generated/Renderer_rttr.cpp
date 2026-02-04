#include "Renderer.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<Renderer>(L"Renderer")
        .constructor<>()
        .method("Create", &Renderer::Create)
        ;
}

