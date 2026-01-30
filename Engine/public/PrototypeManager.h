#pragma once
#include "Manager.h"

NS_BEGIN(Engine)

class PrototypeManager final : public Manager
{
	NO_COPY(PrototypeManager)
public:
	explicit PrototypeManager() = default;
	~PrototypeManager() override = default;

public:
	HRESULT Initialize(Shared<void> arg) override;
	void On_Destroy() override;

public:
	HRESULT			Add_Prototype(uint32 levIndex, const wstring& key, const Shared<Object>& prototype);
	Shared<Object>	Clone_Prototype(PROTOTYPE ePrototype, uint32 levIndex, const wstring& key, Shared<void> arg = nullptr);
	HRESULT			Clear_Prototypes(uint32 levIndex);

private:
	Shared<Object>	Find_Prototype(uint32 levIndex, const wstring& key);

private:
	vector<unordered_map<wstring, Shared<Object>>>	m_Prototypes;
	uint32													m_LevelCount = {};

public:
	static Unique<PrototypeManager> Create(uint32 levCount);
};

NS_END