        #include "GameObjec.h"
        #include <rttr/registration>
        using namespace rttr;
        using namespace Engine;
        RTTR_REGISTRATION
        {
            registration::class_<GameObjec>(L"GameObjec")
            // (
            //     rttr::metadata("parent", L"")
            // )
            .constructor<>()
        }

