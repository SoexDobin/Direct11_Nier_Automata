#include "Game.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Game_RTTR)
{
	registration::class_<Game>("Game")
		.constructor<>()
		;
}
