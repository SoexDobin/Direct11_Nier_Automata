#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL Texture final : public Component
{
public:
	Texture(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Texture(const Shared<Texture>& rhs);
	~Texture() override = default;

public:
	HRESULT Initialize_Prototype(const tChar* textureFilePath, uint32 numSRVs);
	HRESULT Initialize(const Shared<void>& arg) override;
	void On_Destroy() override;

private:
	uint32										m_NumSRVs = {};
	vector<ComPtr<ID3D11ShaderResourceView>>	m_SRVs;

public:
	static Shared<Texture> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<Component> Clone(const Shared<void>& arg) override;
};

NS_END