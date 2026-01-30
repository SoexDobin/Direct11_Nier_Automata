#include "ObjectManager.h"
#include "GameObject.h"

Unique<ObjectManager> ObjectManager::Create()
{
	auto objectManager = make_unique<ObjectManager>();

	if (FAILED(objectManager->Initialize(nullptr)))
	{
		MSG_BOX("Failed To Create ObjectManager");
		return nullptr;
	}

	return objectManager;
}
