#include "pch.h"
#include "Terrain.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{
	registration::class_<Terrain>("Terrain")
		.constructor<>()
		;
}
