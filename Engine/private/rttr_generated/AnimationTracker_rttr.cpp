#include "AnimationTracker.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(AnimationTracker_RTTR)
{
	registration::class_<AnimationTracker>("AnimationTracker")
		.constructor<>()
		.method("Clone", &AnimationTracker::Clone)
		.method("Create", &AnimationTracker::Create)
		.method("CreatePrototype", &AnimationTracker::CreatePrototype)
		;
}
