#include "CameraManager.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<CameraManager>(L"CameraManager")
        .constructor<>()
        .method("Create", &CameraManager::Create)
        ;
}

