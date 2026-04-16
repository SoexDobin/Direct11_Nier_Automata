#include "pch.h"
#include "SparkEffect.h"
#include "VIBuffer_Particle_Point.h"
#include <Game.h>
#include "Random_Helper.h"
#include "SpdLogger.h"

SparkEffect::SparkEffect() : ParticleEffect{} {}
SparkEffect::SparkEffect(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: ParticleEffect{device, context} {}
SparkEffect::SparkEffect(const SparkEffect& rhs)
	: ParticleEffect{rhs} {}

HRESULT SparkEffect::Initialize_Prototype()
{
	if (FAILED(ParticleEffect::Initialize_Prototype()))
	{
		LOG_ERROR(L"Failed to Initialize Proto ParticleEffect");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT SparkEffect::Initialize(void* arg)
{
	if (nullptr == arg)
	{
		LOG_ERROR(L"There is no ParticleEffect Desc");
		return E_FAIL;
	}

	if (FAILED(ParticleEffect::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize ParticleEffect");
		return E_FAIL;
	}

	SPARK_EFFECT_DESC& desc = *static_cast<SPARK_EFFECT_DESC*>(arg);

	if (FAILED(Ready_Components(desc.atkType, desc.position, desc.rotation)))
	{
		LOG_ERROR(L"Failed to Ready_Components ParticleEffect");
		return E_FAIL;
	}

	return S_OK;
}

void SparkEffect::Priority_Update(Float timeDelta)
{
	
}

void SparkEffect::Update(Float timeDelta)
{
	m_Acc += timeDelta;
	
	if (m_Acc >= 0.25f)
	{
		Object::Destroy(shared_from_this());
		return; 
	}

	m_Buffer->Update_Spread(timeDelta);
	m_Transform->Update_WorldMatrix();
}

void SparkEffect::Late_Update(Float timeDelta)
{
	
}

void SparkEffect::Fixed_Update(Float fixedDelta)
{
	
}

HRESULT SparkEffect::Render()
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

HRESULT SparkEffect::Ready_Components(ATK_TYPE atkType, const Vector3& initialPosition, const Quaternion& initialRotation)
{
	Shader::SHADER_DESC shaderDesc{ VTXPARTICLE_POINT_DESC::Tag, VTXPARTICLE_POINT_DESC::Elements, VTXPARTICLE_POINT_DESC::numElements };
	m_Shader = Add_Component<Shader>(ETOI(LEVEL::GAMEPLAY), &shaderDesc);
	if (nullptr == m_Shader)
		return S_OK;

	VIBuffer_Particle_Point::VIBUFFER_INSTANCE_POINT_DESC instanceDesc{};
	instanceDesc.numInstances = 70;
	instanceDesc.isLoop = false;
	instanceDesc.center = Vector3::Zero;
	instanceDesc.pivot = Vector3::Zero;
	instanceDesc.range = Vector3::Zero;
	instanceDesc.scale = Vector2{ 1.f, 3.f };
	instanceDesc.speed = Vector2::Zero;
	instanceDesc.lifeTime = Vector2{ 0.1f, 0.25f };

	m_Buffer = Add_Component<VIBuffer_Particle_Point>(ETOI(LEVEL::GAMEPLAY), &instanceDesc);
	if (nullptr == m_Buffer)
		return S_OK;

	wstring textureTag{};
	if (atkType == ATK_TYPE::POD)
	{
		textureTag = L"Effect_Spark2";
	}
	else
	{
		textureTag = L"Effect_Spark";
	}

	auto textureDesc = Texture::TEXTURE_DESC{ ETOI(LEVEL::GAMEPLAY), textureTag };
	m_Texture = Add_Component<Texture>(ETOI(LEVEL::GAMEPLAY), &textureDesc);
	if (nullptr == m_Texture)
		return E_FAIL;

	m_Transform->Set_Rotation(initialRotation);
	m_Transform->Set_Position(initialPosition);

	return S_OK;
}

HRESULT SparkEffect::Bind_ShaderResources()
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
	if (FAILED(m_Texture->Bind_ShaderResourceView(m_Shader, DiffuseMap, Helper::Random_Int(0, 29))))
		return E_FAIL;

	return S_OK;
}

void SparkEffect::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONLIGHT, shared_from_this());
}

Shared<SparkEffect> SparkEffect::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<SparkEffect>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : SparkEffect");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> SparkEffect::Clone(void* arg)
{
	auto instance = make_shared<SparkEffect>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : SparkEffect");
		return nullptr;
	}

	return instance;
}


