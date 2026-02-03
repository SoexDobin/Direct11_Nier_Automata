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
	uint32 curLayerIndex = 0;
	for (constexpr auto& layer : m_ObjectByLayer)
	{
		if (!m_LayerEnable[curLayerIndex]) continue;

		for (constexpr auto& objects : layer.second)
		{
			objects->Priority_Update(timeDelta);
		}
	}
}

void ObjectManager::Update(Float timeDelta)
{
	uint32 curLayerIndex = 0;
	for (constexpr auto& layer : m_ObjectByLayer)
	{
		if (!m_LayerEnable[curLayerIndex]) continue;

		for (constexpr auto& objects : layer.second)
		{
			objects->Update(timeDelta);
		}
	}
}

void ObjectManager::LateUpdate(Float timeDelta)
{
	uint32 curLayerIndex = 0;
	for (constexpr auto& layer : m_ObjectByLayer)
	{
		if (!m_LayerEnable[curLayerIndex]) continue;

		for (constexpr auto& objects : layer.second)
		{
			objects->Late_Update(timeDelta);
		}
	}
}

void ObjectManager::FixedUpdate(Float fixedDelta)
{
	uint32 curLayerIndex = 0;
	for (constexpr auto& layer : m_ObjectByLayer)
	{
		if (!m_LayerEnable[curLayerIndex]) continue;

		for (constexpr auto& objects : layer.second)
		{
			objects->Fixed_Update(fixedDelta);
		}
	}
}

HRESULT ObjectManager::Add_GameObject(const Shared<GameObject>& object)
{
}

HRESULT ObjectManager::Clear_GameObjects()
{
	for (constexpr auto& layer : m_ObjectByLayer)
		layer.second.clear();
	m_ObjectByLayer.clear();

	for (constexpr auto& type : m_ObjectsByType)
		type.second.clear();
	m_ObjectsByType.clear();

	m_ObjectsByUnique.clear();
	
	m_LayerEnable.fill(true);
}

Shared<GameObject> ObjectManager::Find_GameObjectByType(uint32 typeID)
{
}

Shared<GameObject> ObjectManager::Find_GameObjectByUnique(uint32 objectID)
{
	if (!m_ObjectsByUnique.contains(objectID))
	{
		MSG_BOX("")
	}
}

template <typename T>
constexpr Shared<GameObject> ObjectManager::Find_GameObject()
{
}

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
