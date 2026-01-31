        #include "Time.h"
        #include <rttr/registration>
        using namespace rttr;
        using namespace Engine;
        RTTR_REGISTRATION
        {
            registration::class_<Time>(L"Time")
            // (
            //     rttr::metadata("parent", L"")
            // )
            .constructor<>()
        }

