        #include "Manager.h"
        #include <rttr/registration>
        using namespace rttr;
        using namespace Engine;
        RTTR_REGISTRATION
        {
            registration::class_<Manager>(L"Manager")
            // (
            //     rttr::metadata("parent", L"")
            // )
            .constructor<>()
        }

