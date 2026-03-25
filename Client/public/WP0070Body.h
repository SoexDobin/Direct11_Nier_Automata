#pragma once
#include "P10000Parts.h"

NS_BEGIN(Client)

class CLIENT_DLL WP0070Body final : public P10000Parts
{
	RTTR_ENABLE(P10000Parts)
public:
	explicit WP0070Body();
	explicit WP0070Body(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit WP0070Body(const WP0070Body& rhs);
	~WP0070Body() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;

public:
	void Priority_Update(Float timeDelta) override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void Fixed_Update(Float fixedDelta) override;
	HRESULT Render() override;
	void Submit_RenderGroup() override;

private:
	HRESULT Bind_ShaderResources();
	HRESULT Ready_Components();

public:
	static Shared<WP0070Body> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
};

NS_END
