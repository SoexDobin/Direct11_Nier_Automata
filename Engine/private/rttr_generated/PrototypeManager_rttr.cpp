        #include "PrototypeManager.h"
        #include <rttr/registration>
        using namespace rttr;
        using namespace Engine;
        RTTR_REGISTRATION
        {
            registration::class_<PrototypeManager>(L"PrototypeManager")
            // (
            //     rttr::metadata("parent", L"")
            // )
            .constructor<>()
        }

