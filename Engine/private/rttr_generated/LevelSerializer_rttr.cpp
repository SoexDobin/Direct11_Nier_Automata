#include "LevelSerializer.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(LevelSerializer_RTTR)
{
	registration::class_<LevelSerializer>("LevelSerializer")
		.constructor<>()
		.method("Create", &LevelSerializer::Create)
		;
}
