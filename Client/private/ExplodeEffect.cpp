#include "pch.h"
#include "ExplodeEffect.h"

ExplodeEffect::ExplodeEffect(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: ParticleEffect{device, context} {}
ExplodeEffect::ExplodeEffect(const ExplodeEffect& rhs)
	: ParticleEffect{rhs} {}

HRESULT ExplodeEffect::Initialize_Prototype()
{
	return ParticleEffect::Initialize_Prototype();
}

HRESULT ExplodeEffect::Initialize(void* arg)
{
	return ParticleEffect::Initialize(arg);
}

void ExplodeEffect::Priority_Update(Float timeDelta)
{
	
}

void ExplodeEffect::Update(Float timeDelta)
{
	
}

void ExplodeEffect::Late_Update(Float timeDelta)
{
	
}

void ExplodeEffect::Fixed_Update(Float fixedDelta)
{
	
}

void ExplodeEffect::Submit_RenderGroup()
{
	ParticleEffect::Submit_RenderGroup();
}

HRESULT ExplodeEffect::Render()
{
	return ParticleEffect::Render();
}

HRESULT ExplodeEffect::Ready_Components()
{
	return S_OK;
}

HRESULT ExplodeEffect::Bind_ShaderResources()
{
	return S_OK;
}

Shared<ExplodeEffect> ExplodeEffect::Create(const ComPtr<ID3D11Device>& device,
                                            const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<ExplodeEffect>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : ExplodeEffect");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> ExplodeEffect::Clone(void* arg)
{
	auto instance = make_shared<ExplodeEffect>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : ExplodeEffect");
		return nullptr;
	}

	return instance;
}


