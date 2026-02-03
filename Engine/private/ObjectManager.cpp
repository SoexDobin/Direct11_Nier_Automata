#include "ObjectManager.h"
#include "GameObject.h"

HRESULT ObjectManager::Initialize(Shared<void> arg)
{
	return EngineManager::Initialize(arg);
}

void ObjectManager::On_Destroy()
{
	EngineManager::On_Destroy();
}

void ObjectManager::PriorityUpdate(Float timeDelta)
{
}

void ObjectManager::Update(Float timeDelta)
{
}

void ObjectManager::LateUpdate(Float timeDelta)
{
}

void ObjectManager::FixedUpdate(Float fixedDelta)
{
}

Unique<ObjectManager> ObjectManager::Create(uint32 levIndex)
{
	auto objectManager = make_unique<ObjectManager>();

	if (FAILED(objectManager->Initialize(Shared<void>(&levCount))))
	{
		MSG_BOX("Failed To Create ObjectManager");
		return nullptr;
	}

	return objectManager;
}
