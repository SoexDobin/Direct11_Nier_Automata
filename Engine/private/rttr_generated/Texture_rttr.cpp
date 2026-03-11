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
            metadata("Widget", "ColorPicker")
            )
        ;
}