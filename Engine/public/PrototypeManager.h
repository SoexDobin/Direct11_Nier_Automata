#pragma once
#include "EngineManager.h"
#include "GameObject.h"
#include "String_Helper.h"

NS_BEGIN(Engine)

class GameObject;
class Component;

class PrototypeManager final : public EngineManager
{
	NO_COPY(PrototypeManager)
public:
	explicit PrototypeManager() = default;
	~PrototypeManager() override = default;

public:
	HRESULT Initialize(const Shared<void>& arg) override;
	void On_Destroy() override;

public:
	HRESULT					Create_Reflection(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	HRESULT					Add_Prototype(uint32 levIndex, const Shared<Object>& object, const Shared<void>& arg = nullptr);
	HRESULT					Clear_Prototypes(uint32 levIndex);

	template <typename T>
	constexpr Shared<T>		Clone_Prototype(PROTOTYPE prototype, uint32 levIndex, uint32 typeID, const Shared<void>& arg = nullptr);
	Shared<Object>			Clone_Prototype(PROTOTYPE prototype, uint32 levIndex, uint32 typeID, const Shared<void>& arg = nullptr);
	Shared<Object>			Clone_Prototype(PROTOTYPE prototype, uint32 levIndex, const wstring& typeName, const Shared<void>& arg = nullptr);

public:
	Shared<Object>	Find_Prototype(PROTOTYPE prototype, uint32 levIndex, uint32 typeID) const;
	Shared<Object>	Find_Prototype(PROTOTYPE prototype, uint32 levIndex, const wstring& typeName) const;

private:
	vector<unordered_set<uint32>>								m_Types;
	vector<unordered_map<wstring, uint32>>						m_TypesByName;
	vector<unordered_map<uint32, Shared<GameObject>>>			m_GameObjects;
	vector<unordered_map<uint32, Shared<Component>>>			m_Components;
	uint32														m_LevelCount = {};

public:
	static Unique<PrototypeManager> Create(uint32 levCount);
};

NS_END
