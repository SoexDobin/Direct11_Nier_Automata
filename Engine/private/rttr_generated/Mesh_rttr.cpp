#include "Mesh.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

#line 2511
RTTR_REGISTRATION
{
    registration::class_<Mesh>("Mesh")
        .constructor<>()
        .method("Clone", &Mesh::Clone)
        .method("Create", &Mesh::Create)
        ;
}

