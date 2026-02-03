#include "GameObject.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<GameObject>(L"GameObject")
        .constructor<>()
        .method("Clone", &GameObject::Clone)
        ;
}

