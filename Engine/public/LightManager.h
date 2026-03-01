#pragma once
#include "EngineManager.h"

NS_BEGIN(Engine)

class Light;

class LightManager final : public EngineManager
{
	NO_COPY(LightManager)
public:
	explicit LightManager();
	~LightManager() override = default;

public:
	const LIGHT_DESC* Get_LightDesc(uint32 index) const;
	HRESULT Add_Light(const LIGHT_DESC& desc);
	HRESULT Remove_Light(uint32 index);
	HRESULT Clear_Lights();

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;

public:
	vector<Shared<Light>>	m_Lights;

public:
	static Unique<LightManager> Create();

};

NS_END