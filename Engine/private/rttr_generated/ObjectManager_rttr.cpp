#include "ObjectManager.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

#line 10088
RTTR_REGISTRATION
{
    registration::class_<ObjectManager>("ObjectManager")
        .constructor<>()
        .method("Create", &ObjectManager::Create)
        ;
}

