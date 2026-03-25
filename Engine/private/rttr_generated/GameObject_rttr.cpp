#include "GameObject.h"
#include <rttr/registration>
#include "Game.h"
#include "LayerRegistry.h"
#include "TagRegistry.h"
#include "Engine_RTTR_Metadata.h"

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(GameObject_RTTR)
{
	registration::class_<GameObject>("GameObject") 
		(metadata(Meta_Key_Type::AssetType, Asset_Type_Key::GameObject))
		.method("Clone", &GameObject::Clone)
		;
}
