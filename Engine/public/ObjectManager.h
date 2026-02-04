#pragma once
#include "EngineManager.h"

NS_BEGIN(Engine)

class GameObject;

class ObjectManager final : public EngineManager
{
	NO_COPY(ObjectManager)
public:
	ObjectManager() = default;
	~ObjectManager() override = default;

public:
	uint32 Get_LayerBits() const { return m_LayerMask; }

public:
	HRESULT Initialize(const Shared<void>& arg) override;
	void On_Destroy() override;

public:
	void PriorityUpdate(Float timeDelta);
	void Update(Float timeDelta);
	void LateUpdate(Float timeDelta);
	void FixedUpdate(Float fixedDelta);

	HRESULT Add_GameObject(const Shared<GameObject>& object);
	HRESULT Clear_GameObjects();
	Shared<GameObject> Find_GameObjectByType(uint32 typeID);
	Shared<GameObject> Find_GameObjectByUnique(uint32 objectID);

private:
	map<uint32, list<Shared<GameObject>>>					m_ObjectByLayer;
	unordered_map<uint32, list<Shared<GameObject>>>			m_ObjectByType;
	unordered_map<uint32, Shared<GameObject>>				m_ObjectByUnique;
	uint32													m_LayerMask = { ETOI(LAYER::ALL_LAYER) };

public:
	static Unique<ObjectManager> Create();
};



NS_END