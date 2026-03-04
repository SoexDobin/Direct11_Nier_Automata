#include "VIBuffer_Terrain.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

#line 2001
RTTR_REGISTRATION
{
    registration::class_<VIBuffer_Terrain>("VIBuffer_Terrain")
        .constructor<>()
        .method("Clone", &VIBuffer_Terrain::Clone)
        .method("Create", &VIBuffer_Terrain::Create)
        ;
}

