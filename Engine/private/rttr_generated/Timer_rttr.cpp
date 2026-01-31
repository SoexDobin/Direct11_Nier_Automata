        #include "Timer.h"
        #include <rttr/registration>
        using namespace rttr;
        using namespace Engine;
        RTTR_REGISTRATION
        {
            registration::class_<Timer>(L"Timer")
            // (
            //     rttr::metadata("parent", L"")
            // )
            .constructor<>()
        }

