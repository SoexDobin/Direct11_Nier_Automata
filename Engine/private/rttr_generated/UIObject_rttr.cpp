#include "UIObject.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

#line 10781
RTTR_REGISTRATION {
  registration::class_<UIObject>("UIObject")
      .method("Clone", &UIObject::Clone);
}

