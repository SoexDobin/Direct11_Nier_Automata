#include "pch.h"
#include "ExplodeEffect.h"

#include <SpdLogger.h>
#include <Texture.h>
#include <Game.h>
#include <Shader.h>
#include <VIBuffer_Rect.h>

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
	if (nullptr == arg)
	{
		return E_FAIL;
	}

	auto& desc = *static_cast<EXPLODE_EFFECT_DESC*>(arg);

	m_ThreshHold = desc.threshold;
	m_Acc = 0.f;
	m_CurIndex = 0;

	if (FAILED(ParticleEffect::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Init Explode Effect");
		return E_FAIL;
	}

	if (FAILED(Ready_Components(desc)))
	{
		LOG_ERROR(L"Failed to Ready_Components in ExplodeEffect");
		return E_FAIL;
	}

	return S_OK;
}

void ExplodeEffect::Priority_Update(Float timeDelta)
{
	
}

void ExplodeEffect::Update(Float timeDelta)
{
	if (nullptr == m_Texture)
		return;

	uint32 size = static_cast<uint32>(m_Texture->Get_Textures().size());

	m_Acc += timeDelta;
	if (m_Acc >= m_ThreshHold)
	{
		m_Acc -= m_ThreshHold;
		++m_CurIndex;

		if (m_CurIndex >= size)
		{
			Object::Destroy(shared_from_this());
			return;
		}
	}
}

void ExplodeEffect::Late_Update(Float timeDelta)
{
	Matrix viewMatrixInv = GAME_INSTANCE->Get_InvTransform(D3DTS::VIEW);

	Matrix worldMatrix = m_Transform->Get_WorldMatrix();
	Vector3 scale = m_Transform->Get_Scale();
	Vector3 position = m_Transform->Get_Position();

	worldMatrix.Right(viewMatrixInv.Right() * scale.x);
	worldMatrix.Up(viewMatrixInv.Up() * scale.y);
	worldMatrix.Backward(viewMatrixInv.Backward() * scale.z);
	worldMatrix.Translation(position);

	m_Transform->Set_WorldMatrix(worldMatrix);
	m_Transform->Update_WorldMatrix();
}

void ExplodeEffect::Fixed_Update(Float fixedDelta)
{
	
}

void ExplodeEffect::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

HRESULT ExplodeEffect::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_Shader->Begin(2)))
		return E_FAIL;

	if (FAILED(m_Buffer->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_Buffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT ExplodeEffect::Ready_Components(const EXPLODE_EFFECT_DESC& desc)
{
	uint32 levIndex = GAME_INSTANCE->Get_TargetLevelIndex();

	Shader::SHADER_DESC shaderDesc{ VTXTEX::Tag, VTXTEX::Elements, VTXTEX::numElements };
	m_Shader = Add_Component<Shader>(ETOI(LEVEL::STATIC), &shaderDesc);
	if (nullptr == m_Shader)
		return E_FAIL;

	auto textureDesc = Texture::TEXTURE_DESC{ levIndex, desc.textureTag };
	m_Texture = Add_Component<Texture>(levIndex, &textureDesc);
	if (nullptr == m_Texture)
		return E_FAIL;

	m_Buffer = Add_Component<VIBuffer_Rect>(ETOI(LEVEL::STATIC));
	if (nullptr == m_Buffer)
		return E_FAIL;

	m_Transform->Set_Scale(desc.scale);
	m_Transform->Set_Position(desc.position);

	return S_OK;
}

HRESULT ExplodeEffect::Bind_ShaderResources()
{
	if (FAILED(m_Transform->Bind_ShaderResource(m_Shader, WorldMatrix)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_Shader, ViewMatrix, D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_Shader, ProjMatrix, D3DTS::PROJ)))
		return E_FAIL;

	if (FAILED(m_Texture->Bind_ShaderResourceView(m_Shader, DefaultMap, m_CurIndex)))
		return E_FAIL;

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


