#include "State.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(State_RTTR)
{
	registration::class_<State>("State")
		;
}
