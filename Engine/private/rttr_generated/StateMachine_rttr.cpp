#include "pch.h"
#include "StateMachine.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
	registration::class_<StateMachine>("StateMachine")
		.constructor<>()
		.method("Clone", &StateMachine::Clone)
		;
}
