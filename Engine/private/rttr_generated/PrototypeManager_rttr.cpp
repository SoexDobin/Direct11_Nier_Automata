#include "PrototypeManager.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

#line 5747
RTTR_REGISTRATION
{
    registration::class_<PrototypeManager>("PrototypeManager")
        .constructor<>()
        .method("Create", &PrototypeManager::Create)
        ;
}

