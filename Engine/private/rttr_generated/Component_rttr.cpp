#include "Component.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<Component>(L"Component")
        .constructor<>()
        .method("Clone", &Component::Clone)
        ;
}

