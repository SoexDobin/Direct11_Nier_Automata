#include "Channel.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(Channel_RTTR)
{
	registration::class_<Channel>("Channel")
		.constructor<>()
		.method("Clone", &Channel::Clone)
		.method("Create", &Channel::Create)
		;
}
