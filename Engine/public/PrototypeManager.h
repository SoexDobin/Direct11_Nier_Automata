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
    HRESULT Add_Prototype(uint32 levIndex, const Shared<Object>& object);
    Shared<Object> Find_Prototype(PROTOTYPE prototype, uint32 levIndex, uint32 typeID) const;
	HRESULT Clear_Prototypes();
	HRESULT Clear_Prototypes(uint32 levIndex);

public: /* 읽기 전용 Getter (Game 래핑용) */
    const vector<unordered_map<uint32, Shared<GameObject>>>& Get_GameObjects() const { return m_GameObjects; }
    const vector<unordered_map<uint32, Shared<Component>>>& Get_Components() const { return m_Components; }

private:
    uint32 m_LevelCount = {};
    vector<unordered_map<uint32, Shared<GameObject>>> m_GameObjects;        // ObjectID
    vector<unordered_map<uint32, Shared<Component>>> m_Components;          // ObjectID
    mutable std::recursive_mutex m_PrototypeMutex;

public:
  static Unique<PrototypeManager> Create(uint32 levCount);

private: /* validation method*/
  Bool Validate_Level(uint32 levIndex) const;
};

NS_END
