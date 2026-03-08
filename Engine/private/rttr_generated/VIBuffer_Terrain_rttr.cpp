#include "VIBuffer_Terrain.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(VIBuffer_Terrain_RTTR)
{
    registration::class_<VIBuffer_Terrain>("VIBuffer_Terrain")
        .constructor<>()
        .method("Clone", &VIBuffer_Terrain::Clone)
        .method("Create", &VIBuffer_Terrain::Create)
		.method("CreatePrototype", &VIBuffer_Terrain::CreatePrototype); // 자동 등록 시스템이 찾는 이름
        ;
}

