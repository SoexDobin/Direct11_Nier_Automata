        #include "TimeManager.h"
        #include <rttr/registration>
        using namespace rttr;
        using namespace Engine;
        RTTR_REGISTRATION
        {
            registration::class_<TimeManager>(L"TimeManager")
            // (
            //     rttr::metadata("parent", L"")
            // )
            .constructor<>()
        }

