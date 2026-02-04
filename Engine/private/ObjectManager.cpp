#include "ObjectManager.h"
#include "GameObject.h"

HRESULT ObjectManager::Initialize(const Shared<void>& arg)
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
	for (auto& layer : m_ObjectByLayer)
	{
		if (!m_LayerEnable[curLayerIndex]) continue;

		for (auto& objects : layer.second)
		{
			objects->Priority_Update(timeDelta);
		}
	}
}

void ObjectManager::Update(Float timeDelta)
{
	uint32 curLayerIndex = 0;
	for (auto& layer : m_ObjectByLayer)
	{
		if (!m_LayerEnable[curLayerIndex]) continue;

		for (auto& objects : layer.second)
		{
			objects->Update(timeDelta);
		}
	}
}

void ObjectManager::LateUpdate(Float timeDelta)
{
	uint32 curLayerIndex = 0;
	for (auto& layer : m_ObjectByLayer)
	{
		if (!m_LayerEnable[curLayerIndex]) continue;

		for (auto& objects : layer.second)
		{
			objects->Late_Update(timeDelta);
		}
	}
}

void ObjectManager::FixedUpdate(Float fixedDelta)
{
	uint32 curLayerIndex = 0;
	for (auto& layer : m_ObjectByLayer)
	{
		if (!m_LayerEnable[curLayerIndex]) continue;

		for (auto& objects : layer.second)
		{
			objects->Fixed_Update(fixedDelta);
		}
	}
}

HRESULT ObjectManager::Add_GameObject(const Shared<GameObject>& object)
{
	// TODO : Object의 레이어에 따라서 이스트 추가

	m_ObjectsByType[object->Get_TypeID()].push_back(object);
	m_ObjectsByUnique.emplace(object->Get_ObjectID(), object);

	return S_OK;
}

HRESULT ObjectManager::Clear_GameObjects()
{
	for (auto& layer : m_ObjectByLayer)
		layer.second.clear();
	m_ObjectByLayer.clear();

	for (auto& type : m_ObjectsByType)
		type.second.clear();
	m_ObjectsByType.clear();

	m_ObjectsByUnique.clear();
	
	m_LayerEnable.fill(true);

	return S_OK;
}

Shared<GameObject> ObjectManager::Find_GameObjectByType(uint32 typeID)
{
	if (m_ObjectsByType.contains(typeID) 
		|| m_ObjectsByType[typeID].empty())
	{
		MSG_BOX("Failed To Find GameObject By ObjectID");
		return nullptr;
	}

	return m_ObjectsByType[typeID].front();
}

Shared<GameObject> ObjectManager::Find_GameObjectByUnique(uint32 objectID)
{
	if (!m_ObjectsByUnique.contains(objectID))
	{
		MSG_BOX("Failed To Find GameObject By ObjectID");
		return nullptr;
	}

	return m_ObjectsByUnique[objectID];
}

template <typename T>
constexpr Shared<GameObject> ObjectManager::Find_GameObject()
{
	return nullptr;
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
