#include "Texture.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Texture_RTTR)
{
    registration::class_<Texture>("Texture")
        .constructor<>()
        .method("Clone", &Texture::Clone)
        .method("Create", &Texture::Create)
		.method("CreatePrototype", &Texture::CreatePrototype)

        .property("RGBA", &Texture::Get_RGBAByValue, &Texture::Set_RGBA)(
            metadata(Meta_Key::DataType, "Color"),
            metadata(Meta_Key::Widget, Widget_Type::ColorPicker),
            metadata(Meta_Key::SaveData, Serialize_Data_Field::ColorVector4)
            )
        .property("TextureTag", &Texture::Get_TextureTag, &Texture::Set_TextureTag)
        (
            metadata(Meta_Key::DataType, "wstring"),
            metadata(Meta_Key::Widget, Widget_Type::AssetDrop),
            metadata(Meta_Key::AssetType, "Texture"),
            metadata(Meta_Key::SaveData, Serialize_Data_Field::TextureTag)
        )
        ;
}