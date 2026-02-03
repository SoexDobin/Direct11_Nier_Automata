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
	HRESULT Initialize(Shared<void> arg) override;
	void On_Destroy() override;

public:
	void PriorityUpdate(Float timeDelta);
	void Update(Float timeDelta);
	void LateUpdate(Float timeDelta);
	void FixedUpdate(Float fixedDelta);

	HRESULT Add_Object(uint32 levIndex, const Shared<GameObject>& object);
	HRESULT Clear_Objects(uint32 levIndex);
	Shared<GameObject> Find_Object(uint32 levIndex, uint32 objectID);

private:
	unordered_map<uint32, list<Shared<GameObject>>>		m_ObjectsByType;
	unordered_map<uint32, Shared<GameObject>>			m_ObjectsByUnique;
	array<Bool, 32>										m_LayerEnable = { false... };

public:
	static Unique<ObjectManager> Create(uint32 levIndex);
};

NS_END