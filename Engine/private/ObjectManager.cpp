#include "ObjectManager.h"
#include "GameObject.h"

HRESULT ObjectManager::Initialize(const Shared<void>& arg)
{
	m_LayerMask = ETOI(LAYER::ALL_LAYER);

	return EngineManager::Initialize(arg);
}

void ObjectManager::On_Destroy()
{
	Clear_GameObjects();

	EngineManager::On_Destroy();
}

void ObjectManager::PriorityUpdate(Float timeDelta)
{
	for (auto& [layerBit, objects] : m_ObjectByLayer)
	{
		if ((m_LayerMask & layerBit) == 0)
			continue;

		for (auto it = objects.begin(); it != objects.end(); )
		{
			auto& obj = *it;
			
			if (obj->Is_Destroy())
			{
				m_ObjectByType[obj->Get_TypeID()].remove(obj);
				m_ObjectByUnique.erase(obj->Get_ObjectID());
				it = objects.erase(it);
				continue;
			}

			if (obj->Is_Active())
				obj->Priority_Update(timeDelta);

			++it;
		}
	}
}

void ObjectManager::Update(Float timeDelta)
{
	for (auto& [layerBit, objects] : m_ObjectByLayer)
	{
		if ((m_LayerMask & layerBit) == 0)
			continue;

		for (auto& obj : objects)
		{
			if (obj->Is_Active())
				obj->Update(timeDelta);
		}
	}
}

void ObjectManager::LateUpdate(Float timeDelta)
{
	for (auto& [layerBit, objects] : m_ObjectByLayer)
	{
		if ((m_LayerMask & layerBit) == 0)
			continue;

		for (auto& obj : objects)
		{
			if (obj->Is_Active())
				obj->Late_Update(timeDelta);
		}
	}
}

void ObjectManager::FixedUpdate(Float fixedDelta)
{
	for (auto& [layerBit, objects] : m_ObjectByLayer)
	{
		if ((m_LayerMask & layerBit) == 0)
			continue;

		for (auto& obj : objects)
		{
			if (obj->Is_Active())
				obj->Fixed_Update(fixedDelta);
		}
	}
}

HRESULT ObjectManager::Add_GameObject(const Shared<GameObject>& object)
{
	m_ObjectByLayer[object->Get_LayerMask().Get_Layer()].push_back(object);
	m_ObjectByType[object->Get_TypeID()].push_back(object);
	m_ObjectByUnique.emplace(object->Get_ObjectID(), object);

	return S_OK;
}

HRESULT ObjectManager::Clear_GameObjects()
{
	for (auto& layer : m_ObjectByLayer)
		layer.second.clear();
	m_ObjectByType.clear();

	for (auto& type : m_ObjectByType)
		type.second.clear();
	m_ObjectByUnique.clear();

	m_LayerMask = ETOI(LAYER::ALL_LAYER);
	return S_OK;
}

Shared<GameObject> ObjectManager::Find_GameObjectByType(uint32 typeID)
{
	if (!m_ObjectByType.contains(typeID)
		|| m_ObjectByType[typeID].empty())
	{
		MSG_BOX("Failed To Find GameObject By ObjectID");
		return nullptr;
	}

	return m_ObjectByType[typeID].front();
}

Shared<GameObject> ObjectManager::Find_GameObjectByUnique(uint32 objectID)
{
	if (!m_ObjectByUnique.contains(objectID))
	{
		MSG_BOX("Failed To Find GameObject By ObjectID");
		return nullptr;
	}

	return m_ObjectByUnique[objectID];
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
