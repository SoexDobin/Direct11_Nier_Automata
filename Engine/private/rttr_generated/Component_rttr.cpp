        #include "Component.h"
        #include <rttr/registration>
        using namespace rttr;
        using namespace Engine;
        RTTR_REGISTRATION
        {
            registration::class_<Component>(L"Component")
            // (
            //     rttr::metadata("parent", L"")
            // )
            .constructor<>()
        }

