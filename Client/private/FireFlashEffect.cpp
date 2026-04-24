#include "pch.h"
#include "FireFlashEffect.h"
#include "VIBuffer_Particle_Point.h"
#include <Game.h>
#include "Random_Helper.h"
#include "SpdLogger.h"

FireFlashEffect::FireFlashEffect() : ParticleEffect{} {}
FireFlashEffect::FireFlashEffect(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: ParticleEffect{device, context} {}
FireFlashEffect::FireFlashEffect(const FireFlashEffect& rhs)
	: ParticleEffect{rhs} {}

HRESULT FireFlashEffect::Initialize_Prototype()
{
	if (FAILED(ParticleEffect::Initialize_Prototype()))
	{
		LOG_ERROR(L"Failed to Initialize Proto ParticleEffect");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT FireFlashEffect::Initialize(void* arg)
{
	if (nullptr == arg)
	{
		LOG_ERROR(L"There is no ParticleEffect Desc");
		return S_OK;
	}

	if (FAILED(ParticleEffect::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize ParticleEffect");
		return E_FAIL;
	}

	FIRE_FLASH_EFFECT_DESC& desc = *static_cast<FIRE_FLASH_EFFECT_DESC*>(arg);

	if (FAILED(Ready_Components(desc.parentMatrix)))
	{
		LOG_ERROR(L"Failed to Ready_Components ParticleEffect");
		return E_FAIL;
	}

	return S_OK;
}

void FireFlashEffect::Priority_Update(Float timeDelta)
{
	
}

void FireFlashEffect::Update(Float timeDelta)
{
	m_Buffer->Update_Spread(timeDelta);
	m_Transform->Update_WorldMatrix();
}

void FireFlashEffect::Late_Update(Float timeDelta)
{
	
}

void FireFlashEffect::Fixed_Update(Float fixedDelta)
{
	
}

HRESULT FireFlashEffect::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_Shader->Begin(0)))
		return E_FAIL;

	if (FAILED(m_Buffer->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_Buffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT FireFlashEffect::Ready_Components(const Matrix& parentMatrix)
{
	uint32 levIndex = GAME_INSTANCE->Get_TargetLevelIndex();

	Shader::SHADER_DESC shaderDesc{ VTXPARTICLE_POINT_DESC::Tag, VTXPARTICLE_POINT_DESC::Elements, VTXPARTICLE_POINT_DESC::numElements };
	m_Shader = Add_Component<Shader>(ETOI(LEVEL::STATIC), &shaderDesc);
	if (nullptr == m_Shader)
		return S_OK;

	VIBuffer_Particle_Point::VIBUFFER_INSTANCE_POINT_DESC instanceDesc{};
	instanceDesc.numInstances = 3;
	instanceDesc.isLoop = true;
	instanceDesc.center = Vector3::Zero;
	instanceDesc.pivot = Vector3::Zero;
	instanceDesc.range = Vector3::Zero;
	instanceDesc.scale = Vector2{ 0.5f, 1.f };
	instanceDesc.speed = Vector2::Zero;
	instanceDesc.lifeTime = Vector2{ 0.1f, 0.2f };

	m_Buffer = Add_Component<VIBuffer_Particle_Point>(levIndex, &instanceDesc);
	if (nullptr == m_Buffer)
		return S_OK;

	auto textureDesc = Texture::TEXTURE_DESC{ levIndex, L"Fire_Flash" };
	m_Texture = Add_Component<Texture>(levIndex, &textureDesc);
	if (nullptr == m_Texture)
		return E_FAIL;

	m_Transform->Set_WorldMatrix(parentMatrix);

	return S_OK;
}

HRESULT FireFlashEffect::Bind_ShaderResources()
{
	uint32 isLocked = 1;

	if (FAILED(m_Transform->Bind_ShaderResource(m_Shader, WorldMatrix)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_Shader, ViewMatrix, D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_Shader, ProjMatrix, D3DTS::PROJ)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_CameraPosition(m_Shader, CameraPosition)))
		return E_FAIL;



	if (FAILED(m_Shader->Bind_RawValue("g_LockUpRight", &isLocked, sizeof(uint32))))
		return E_FAIL;
	if (FAILED(m_Texture->Bind_ShaderResourceView(m_Shader, DiffuseMap, 0)))
		return E_FAIL;

	return S_OK;
}

void FireFlashEffect::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::BLEND, shared_from_this());
}

Shared<FireFlashEffect> FireFlashEffect::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<FireFlashEffect>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : FireFlashEffect");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> FireFlashEffect::Clone(void* arg)
{
	auto instance = make_shared<FireFlashEffect>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : FireFlashEffect");
		return nullptr;
	}

	return instance;
}


