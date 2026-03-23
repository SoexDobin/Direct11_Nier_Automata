#include "Texture.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

using namespace Meta_Key_Type;
using namespace Data_Type_Key;
using namespace Data_Tag;
using namespace Asset_Type_Key;
using namespace Save_Data_Key;

RTTR_REGISTRATION_NAMED(Texture_RTTR)
{
	registration::class_<Texture>("Texture")
		(metadata(Meta_Key_Type::AssetType, Asset_Type_Key::Texture))
		.constructor<>()
		.method("Clone", &Texture::Clone)
		.method("Create", &Texture::Create)
		.method("CreatePrototype", &Texture::CreatePrototype)
		.property("RGBA", &Texture::Get_RGBAByValue, &Texture::Set_RGBA)
		(rttr::metadata(DataType, Data_Type_Key::Vector4),
			rttr::metadata(TypeTag, Data_Tag::Color),
			rttr::metadata(SaveData, TransformScale))
		.property("ResourceTag", &Texture::Get_TextureTag, &Texture::Set_TextureTag)
		(
			metadata(DataType, wString),
			metadata(TypeTag, Data_Tag::ResourceTag),
			metadata(SaveData, Save_Data_Key::TextureTag))
		;
}
