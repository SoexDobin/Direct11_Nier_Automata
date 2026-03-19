#include "Model.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Model_RTTR)
{
    registration::class_<Model>("Model")
        .constructor<>()
        .method("Clone", &Model::Clone)
        .method("Create", &Model::Create)
		.method("CreatePrototype", &Model::CreatePrototype)
        .property("ModelTag", &Model::Get_ModelTag, &Model::Set_ModelTag)
        (
            metadata("SaveData", "AssetDrop"),
            metadata("AssetType", "Model")
        )
        ;
}

