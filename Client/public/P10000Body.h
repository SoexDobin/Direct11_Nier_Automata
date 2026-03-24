#pragma once
#include "P10000Parts.h"

NS_BEGIN(Engine)
class Model;
NS_END

NS_BEGIN(Client)

class WP0220Body;
class WP0070Body;

class CLIENT_DLL P10000Body final : public P10000Parts
{
	RTTR_ENABLE(PartObject)
public:
	typedef struct tagP10000BodyDesc : public PARTOBJECT_DESC
	{} P10000BODY_DESC ;

public:
	explicit P10000Body();
	explicit P10000Body(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit P10000Body(const P10000Body& rhs);
	~P10000Body() override = default;

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

private:
	int32 rootBoneIndex{};

	Shared<WP0070Body> m_Sword{ nullptr };
	Shared<WP0220Body> m_GreaterSword{ nullptr };

public:
	static Shared<P10000Body> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
};

NS_END
