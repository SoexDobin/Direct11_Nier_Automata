        #include "ObjectManager.h"
        #include <rttr/registration>
        using namespace rttr;
        using namespace Engine;
        RTTR_REGISTRATION
        {
            registration::class_<ObjectManager>(L"ObjectManager")
            // (
            //     rttr::metadata("parent", L"")
            // )
            .constructor<>()
        }

