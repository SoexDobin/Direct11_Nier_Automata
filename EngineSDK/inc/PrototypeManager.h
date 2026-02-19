#pragma once
#include "EngineManager.h"
#include "GameObject.h"
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
  uint32 Get_TypeByName(const wstring &name);
  const wstring &Get_NameByType(uint32 typeID);

public:
  HRESULT Initialize(void *arg) override;
  void On_Destroy() override;

public:
  HRESULT Create_Reflection(const ComPtr<ID3D11Device> &device,
                            const ComPtr<ID3D11DeviceContext> &context);
  HRESULT Add_Prototype(uint32 levIndex, const Shared<Object> &object,
                        void *arg = nullptr);
  HRESULT Clear_Prototypes(uint32 levIndex);

public: /* Prefab 직렬화 */
  HRESULT Export_Prefabs(const wstring &path);
  HRESULT Import_Prefabs(const wstring &path);

public: /* 읽기 전용 Getter (Game 래핑용) */
  const vector<unordered_map<uint32, Shared<GameObject>>> &
  Get_GameObjects() const {
    return m_GameObjects;
  }
  const vector<unordered_map<uint32, Shared<Component>>> &
  Get_Components() const {
    return m_Components;
  }
  const vector<unordered_map<uint32, wstring>> &Get_NameByTypes() const {
    return m_NameByTypes;
  }

public:
  Shared<Object> Find_Prototype(PROTOTYPE prototype, uint32 levIndex,
                                uint32 typeID) const;
  Shared<Object> Find_Prototype(PROTOTYPE prototype, uint32 levIndex,
                                const wstring &typeName) const;

private:
  vector<unordered_map<uint32, wstring>> m_NameByTypes;
  vector<unordered_map<wstring, uint32>> m_TypesByName;
  vector<unordered_map<uint32, Shared<GameObject>>> m_GameObjects;
  vector<unordered_map<uint32, Shared<Component>>> m_Components;
  uint32 m_LevelCount = {};

private:
  void Register_Type(rttr::type type, PROTOTYPE protoType,
                     const ComPtr<ID3D11Device> &device,
                     const ComPtr<ID3D11DeviceContext> &context);

public:
  static Unique<PrototypeManager> Create(uint32 levCount);

private: /* validation method*/
  Bool Validate_Level(uint32 levIndex) const;
};

NS_END
