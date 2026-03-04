#include "Texture.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

#line 5763
RTTR_REGISTRATION
{
    registration::class_<Texture>("Texture")
        .constructor<>()
        .method("Clone", &Texture::Clone)
        .method("Create", &Texture::Create)
        ;
}

