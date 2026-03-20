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
            metadata(Meta_Key::DataType, "wstring"),
            metadata(Meta_Key::Widget, Widget_Type::AssetDrop),
            metadata(Meta_Key::AssetType, "Model"),
            metadata(Meta_Key::SaveData, Serialize_Data_Field::ModelTag)
        )
        ;
}
