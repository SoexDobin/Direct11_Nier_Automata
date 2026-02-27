#include "Pipeline.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<Pipeline>(L"Pipeline")
        .constructor<>()
        .method("Create", &Pipeline::Create)
        ;
}

