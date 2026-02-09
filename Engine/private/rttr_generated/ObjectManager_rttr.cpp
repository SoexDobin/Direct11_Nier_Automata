#include "ObjectManager.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<ObjectManager>(L"ObjectManager")
        .constructor<>()
        .method("Create", &ObjectManager::Create)
        ;
}

