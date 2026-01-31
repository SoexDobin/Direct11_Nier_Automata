    #include "WINMODE.h"
    #include <rttr/registration>
    using namespace rttr;
    using namespace Engine;
    RTTR_REGISTRATION
    {
        registration::class_<WINMODE>(L"WINMODE")
        // (
        //     rttr::metadata("parent", L"")
        // )
        .constructor<>()
    }
