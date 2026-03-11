#include "SimpleMath.h"
#include <rttr/registration>
#include <Engine_Typedef.h>
using namespace rttr;
using namespace Engine;

#line 9942
RTTR_REGISTRATION
{
    using namespace rttr;
    using namespace Engine; // Engine::Vector2 ???ъ슜
    registration::class_<Vector2>("Vector2")
        .property("x", &Vector2::x).property("y", &Vector2::y);
    registration::class_<Vector3>("Vector3")
        .property("x", &Vector3::x).property("y", &Vector3::y).property("z", &Vector3::z);
    registration::class_<Vector4>("Vector4")
        .property("x", &Vector4::x).property("y", &Vector4::y).property("z", &Vector4::z).property("w", &Vector4::w);
    registration::class_<Quaternion>("Quaternion")
        .property("x", &Quaternion::x).property("y", &Quaternion::y)
        .property("z", &Quaternion::z).property("w", &Quaternion::w);

    registration::class_<Color>("Color")
        .property("R", &Color::R).property("G", &Color::G)
        .property("B", &Color::B).property("A", &Color::A);
}


