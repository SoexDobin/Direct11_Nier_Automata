        #include "Object.h"
        #include <rttr/registration>
        using namespace rttr;
        using namespace Engine;
        RTTR_REGISTRATION
        {
            registration::class_<Object>(L"Object")
            // (
            //     rttr::metadata("parent", L"")
            // )
            .constructor<>()
        }

