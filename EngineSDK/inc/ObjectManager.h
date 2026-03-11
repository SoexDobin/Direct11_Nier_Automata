#pragma once
#include "EngineManager.h"

NS_BEGIN(Engine)

class GameObject;

class ObjectManager final : public EngineManager {
  NO_COPY(ObjectManager)

public:
  ObjectManager() = default;
  ~ObjectManager() override = default;

public:
  uint32 Get_LayerBits() const { return m_LayerMask; }

public:
  HRESULT Initialize(void *arg) override;
  void On_Destroy() override;

public:
  void PriorityUpdate(Float timeDelta);
  void Update(Float timeDelta);
  void LateUpdate(Float timeDelta);
  void FixedUpdate(Float fixedDelta);
  void Submit_RenderGroup();
  void Cleanup_GameObjects();

public:
  HRESULT Add_GameObject(const Shared<GameObject> &object);
  HRESULT Clear_GameObjects();
  Shared<GameObject> Find_ObjectByType(uint32 typeID);
  const vector<Shared<GameObject>> &Find_ObjectsByTypes(uint32 typeID);
  Shared<GameObject> Find_ObjectByObjectID(uint32 objectID);
  const vector<Shared<GameObject>> &Find_ObjectsByObjectID(uint32 objectID);
  Shared<GameObject> Find_ByInstanceID(uint32 instanceID);
  const unordered_map<uint32, Shared<GameObject>> &Get_GameObjects();

private:
  map<uint32, vector<Shared<GameObject>>> m_ObjectByLayer;
  unordered_map<uint32, vector<Shared<GameObject>>> m_ObjectByType;
  unordered_map<uint32, vector<Shared<GameObject>>> m_ObjectByObject;
  unordered_map<uint32, Shared<GameObject>> m_ObjectByInstance;
  uint32 m_LayerMask = {ETOI(LAYER::ALL_LAYER)};

public:
  static Unique<ObjectManager> Create();
};

NS_END