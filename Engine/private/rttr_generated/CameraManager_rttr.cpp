#include "CameraManager.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

#line 5819
RTTR_REGISTRATION
{
    registration::class_<CameraManager>("CameraManager")
        .constructor<>()
        .method("Create", &CameraManager::Create)
        ;
}

