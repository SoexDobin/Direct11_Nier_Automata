#pragma once
#include "EngineManager.h"

NS_BEGIN(Engine)

class PrototypeManager final : public EngineManager
{
	NO_COPY(PrototypeManager)
public:
	explicit PrototypeManager() = default;
	~PrototypeManager() override = default;

public:
	HRESULT Initialize(Shared<void> arg) override;
	void On_Destroy() override;

public:
	HRESULT			Create_Reflection(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	HRESULT			Add_Prototype(uint32 levIndex, const Shared<Object>& prototype);
	Shared<Object>	Clone_Prototype(uint32 levIndex, Shared<void> arg = nullptr);
	HRESULT			Clear_Prototypes(uint32 levIndex);

private:
	Shared<Object>	Find_Prototype(uint32 levIndex, const wstring& typeTag);
	template <typename T>
	uint32			Find_TypeID();
	uint32			Find_TypeID(const wstring& typeTag);

private:
	
	vector<map<wstring, uint32>>					m_Types;
	vector<unordered_map<uint32, Shared<Object>>>	m_Prototypes;
	uint32											m_LevelCount = {};

public:
	static Unique<PrototypeManager> Create(uint32 levCount);
};

NS_END