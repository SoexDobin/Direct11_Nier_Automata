#include "LevelManager.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

#line 6030
RTTR_REGISTRATION
{
    registration::class_<LevelManager>("LevelManager")
        .constructor<>()
        .method("Create", &LevelManager::Create)
        ;
}

