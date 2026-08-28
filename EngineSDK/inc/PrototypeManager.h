#pragma once
#include "EngineManager.h"
#include "Object.h"
#include "String_Helper.h"

NS_BEGIN(Engine)

class GameObject;
class Component;

class PrototypeManager final : public EngineManager {
	NO_COPY(PrototypeManager)
public:
	explicit PrototypeManager() = default;
	~PrototypeManager() override = default;

public:
	HRESULT Initialize(void *arg) override;
	void On_Destroy() override;

public:
    uint32 Get_ObjectIDFromPrototypeTag(const wstring &prototypeTag, uint32 levIndex) const;
    wstring Get_PrototypeTagFromObjectID(uint32 objectID, uint32 levIndex) const;
    HRESULT Add_Prototype(uint32 levIndex, const Shared<Object> &object, const wstring &prototypeTag);
    Shared<Object> Find_Prototype(PROTOTYPE prototype, uint32 levIndex, uint32 objectID) const;
    Shared<Object> Find_DefaultPrototype(PROTOTYPE prototype, uint32 levIndex, RuntimeTypeId runtimeTypeId) const;
    Shared<Object> Find_Prototype(PROTOTYPE prototype, uint32 levIndex, const wstring &prototypeTag) const;
    HRESULT Clear_Prototypes();
    HRESULT Clear_Prototypes(uint32 levIndex);

public: /* Read Only */
    const vector<unordered_map<uint32, Shared<GameObject>>>&
    Get_GameObjects() const { return m_GameObjects; }
    const vector<unordered_map<uint32, Shared<Component>>>&
    Get_Components() const { return m_Components; }

private:
    HRESULT Register_EngineComponents();

private:
    uint32 m_LevelCount = {};
    vector<unordered_map<wstring, uint32>> m_ObjectsID;
    vector<unordered_map<uint32, Shared<GameObject>>> m_GameObjects; // ObjectID
    vector<unordered_map<uint32, Shared<Component>>> m_Components;   // ObjectID
    vector<unordered_map<RuntimeTypeId, uint32>> m_DefaultPrototypeIDs;
    mutable std::recursive_mutex m_PrototypeMutex;

public:
	static Unique<PrototypeManager> Create(uint32 levCount);

private: /* validation method*/
	Bool Validate_Level(uint32 levIndex) const;
};

NS_END
