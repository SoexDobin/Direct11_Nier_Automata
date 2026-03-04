#include "UIObject.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

#line 10781
RTTR_REGISTRATION {
  registration::class_<UIObject>("UIObject")
      .property("X", &UIObject::m_X)
      .property("Y", &UIObject::m_Y)
      .property("SizeX", &UIObject::m_SizeX)
      .property("SizeY", &UIObject::m_SizeY)
      .property("ShaderPath", &UIObject::m_ShaderPath)
      .property("TexturePath", &UIObject::m_TexturePath)
      .property("NumSRV", &UIObject::m_NumSRV)
      .method("Clone", &UIObject::Clone);
}

