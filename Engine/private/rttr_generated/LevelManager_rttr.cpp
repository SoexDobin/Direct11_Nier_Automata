        #include "LevelManager.h"
        #include <rttr/registration>
        using namespace rttr;
        using namespace Engine;
        RTTR_REGISTRATION
        {
            registration::class_<LevelManager>(L"LevelManager")
            // (
            //     rttr::metadata("parent", L"")
            // )
            .constructor<>()
        }

