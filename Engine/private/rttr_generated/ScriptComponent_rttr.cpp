#include "ScriptComponent.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

#line 5505
RTTR_REGISTRATION
{
    registration::class_<ScriptComponent>("ScriptComponent")
        .constructor<>()
        .method("Clone", &ScriptComponent::Clone)
        ;
}

