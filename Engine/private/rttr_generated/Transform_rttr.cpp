#include "Transform.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<Transform>(L"Transform")
        .constructor<>()
        .method("Clone", &Transform::Clone)
        .method("Create", &Transform::Create)
        ;
}

