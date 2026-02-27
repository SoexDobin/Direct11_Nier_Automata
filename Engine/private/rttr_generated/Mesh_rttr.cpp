#include "Mesh.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<Mesh>(L"Mesh")
        .constructor<>()
        .method("Clone", &Mesh::Clone)
        .method("Create", &Mesh::Create)
        ;
}

