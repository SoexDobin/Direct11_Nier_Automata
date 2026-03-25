#include "ScriptComponent.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(ScriptComponent_RTTR)
{
	registration::class_<ScriptComponent>("ScriptComponent")
		.method("Clone", &ScriptComponent::Clone)
		;
}
