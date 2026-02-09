#include "Game.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
    registration::class_<Game>(L"Game")
        .constructor<>()
        ;
}

