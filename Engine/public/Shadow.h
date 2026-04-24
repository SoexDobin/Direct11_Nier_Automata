#pragma once
#include "EngineManager.h"

NS_BEGIN(Engine)

class Shader;

class Shadow final : public EngineManager
{
	NO_COPY(Shadow)
public:
	explicit Shadow() = default;
	~Shadow() override = default;

public:
	HRESULT Add_ShadowLight(const SHADOW_LIGHT_DESC& desc);
	HRESULT Bind_TransformMatrix(const Shared<Shader>& shader, const Char* constantName, D3DTS transformState) const;

public:
	HRESULT Initialize_Prototype() override { return EngineManager::Initialize_Prototype(); }
	HRESULT Initialize(void* arg) override { return EngineManager::Initialize(arg); }
	void On_Destroy() override { EngineManager::On_Destroy(); }

private:
	Matrix m_TransformMatrices[ETOI(D3DTS::END)] = {};

public:
	static Unique<Shadow> Create();
};

NS_END
