#include "StateMachine.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(StateMachine_RTTR)
{
	registration::class_<StateMachine>("StateMachine")
		.constructor<>()
		.method("Clone", &StateMachine::Clone)
		;
}
