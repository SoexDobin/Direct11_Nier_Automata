#include "Texture.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<Texture>(L"Texture")
        .constructor<>()
        .method("Clone", &Texture::Clone)
        .method("Create", &Texture::Create)
        ;
}

