        #include "GraphicDevic.h"
        #include <rttr/registration>
        using namespace rttr;
        using namespace Engine;
        RTTR_REGISTRATION
        {
            registration::class_<GraphicDevic>(L"GraphicDevic")
            // (
            //     rttr::metadata("parent", L"")
            // )
            .constructor<>()
        }

