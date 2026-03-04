#include "Model.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

#line 2880
RTTR_REGISTRATION
{
    registration::class_<Model>("Model")
        .constructor<>()
        .method("Clone", &Model::Clone)
        .method("Create", &Model::Create)
        ;
}

