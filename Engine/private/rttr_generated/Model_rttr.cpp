#include "Model.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

using namespace Meta_Key_Type;
using namespace Data_Type_Key;
using namespace Data_Tag;
using namespace Asset_Type_Key;
using namespace Save_Data_Key;

RTTR_REGISTRATION_NAMED(Model_RTTR)
{
	registration::class_<Model>("Model")
		(metadata(Meta_Key_Type::AssetType, Asset_Type_Key::Model))
		.constructor<>()
		.method("Clone", &Model::Clone)
		.method("Create", &Model::Create)
		.method("CreatePrototype", &Model::CreatePrototype)
		.property("ModelTag", &Model::Get_ModelTag, &Model::Set_ModelTag)
		(
			metadata(DataType, wString),
			metadata(TypeTag, Data_Tag::ResourceTag),
			metadata(SaveData, Save_Data_Key::ModelTag)
		)
		.property("AnimationPreset", &Model::Get_AnimationPreset, &Model::Set_AnimationPreset)
		(
			metadata(DataType, "AnimationPreset"),
			metadata(TypeTag, "AnimationPreset"),
			metadata(SaveData, Save_Data_Key::AnimationPreset)
		);
}
