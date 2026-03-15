#include "pch.h"
#include "Channel.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
	registration::class_<Channel>("Channel")
		.constructor<>()
		.method("Clone", &Channel::Clone)
		.method("Create", &Channel::Create)
		;
}
