#include "Model.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<Model>(L"Model")
        .constructor<>()
        .method("Clone", &Model::Clone)
        .method("Create", &Model::Create)
        ;
}

