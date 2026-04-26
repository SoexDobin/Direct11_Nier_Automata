#include "pch.h"
#include "ExplodeEffect_Instance.h"

#include <SpdLogger.h>
#include <Texture.h>
#include <Game.h>
#include <Shader.h>
#include <VIBuffer_Particle_Point.h>

ExplodeEffect_Instance::ExplodeEffect_Instance(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: ParticleEffect{device, context} {}

ExplodeEffect_Instance::ExplodeEffect_Instance(const ExplodeEffect_Instance& rhs)
	: ParticleEffect{rhs} {}

HRESULT ExplodeEffect_Instance::Initialize_Prototype()
{
	return ParticleEffect::Initialize_Prototype();
}

HRESULT ExplodeEffect_Instance::Initialize(void* arg)
{
	if (nullptr == arg)
	{
		return E_FAIL;
	}

	auto& desc = *static_cast<EXPLODE_EFFECT_INSTANCE_DESC*>(arg);

	m_ThreshHold = desc.threshold;
	m_Acc = 0.f;
	m_CurIndex = 0;

	if (FAILED(ParticleEffect::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Init ExplodeEffect_Instance");
		return E_FAIL;
	}

	if (FAILED(Ready_Components(desc)))
	{
		LOG_ERROR(L"Failed to Ready_Components in ExplodeEffect_Instance");
		return E_FAIL;
	}

	// 첫 프레임 스케일 및 렌더링 오류 방지
	m_Transform->Update_WorldMatrix();

	return S_OK;
}

void ExplodeEffect_Instance::Priority_Update(Float timeDelta)
{
}

void ExplodeEffect_Instance::Update(Float timeDelta)
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

void ExplodeEffect_Instance::Late_Update(Float timeDelta)
{
	m_Transform->Update_WorldMatrix();
}

void ExplodeEffect_Instance::Fixed_Update(Float fixedDelta)
{
}

void ExplodeEffect_Instance::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONLIGHT, shared_from_this());
}

HRESULT ExplodeEffect_Instance::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_Shader->Begin(1)))
		return E_FAIL;

	if (FAILED(m_Buffer->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_Buffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT ExplodeEffect_Instance::Ready_Components(const EXPLODE_EFFECT_INSTANCE_DESC& desc)
{
	uint32 levIndex = GAME_INSTANCE->Get_TargetLevelIndex();

	// 인스턴싱용 쉐이더 컴포넌트 추가
	Shader::SHADER_DESC shaderDesc{ VTXPARTICLE_POINT_DESC::Tag, VTXPARTICLE_POINT_DESC::Elements, VTXPARTICLE_POINT_DESC::numElements };
	m_Shader = Add_Component<Shader>(ETOI(LEVEL::STATIC), &shaderDesc);
	if (nullptr == m_Shader)
		return E_FAIL;

	// 텍스쳐 컴포넌트 추가
	auto textureDesc = Texture::TEXTURE_DESC{ levIndex, desc.textureTag };
	m_Texture = Add_Component<Texture>(levIndex, &textureDesc);
	if (nullptr == m_Texture)
		return E_FAIL;

	// 인스턴싱 버퍼 초기화 구조체 세팅
	VIBuffer_Particle_Point::VIBUFFER_INSTANCE_POINT_DESC bufferDesc{};
	bufferDesc.numInstances = desc.instanceCount;
	bufferDesc.center = Vector3::Zero;  // Transform을 기준으로 상대적으로 흩어지므로 Zero
	bufferDesc.range = desc.range;
	bufferDesc.scale = desc.scaleRange;
	bufferDesc.pivot = Vector3::Zero;
	bufferDesc.speed = Vector2{0.f, 0.f};
	bufferDesc.lifeTime = Vector2{0.f, 0.f};
	bufferDesc.isLoop = false;

	// 파티클 인스턴스 버퍼 생성
	m_Buffer = Add_Component<VIBuffer_Particle_Point>(ETOI(LEVEL::STATIC), &bufferDesc);
	if (nullptr == m_Buffer)
		return E_FAIL;

	m_Transform->Set_Position(desc.position);

	return S_OK;
}

HRESULT ExplodeEffect_Instance::Bind_ShaderResources()
{
	if (FAILED(m_Transform->Bind_ShaderResource(m_Shader, WorldMatrix)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_Shader, ViewMatrix, D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_Shader, ProjMatrix, D3DTS::PROJ)))
		return E_FAIL;

	// VtxParticlePoint 쉐이더는 g_DiffuseTexture를 사용하므로 DiffuseMap으로 바인딩
	if (FAILED(m_Texture->Bind_ShaderResourceView(m_Shader, DiffuseMap, m_CurIndex)))
		return E_FAIL;

	return S_OK;
}

Shared<ExplodeEffect_Instance> ExplodeEffect_Instance::Create(const ComPtr<ID3D11Device>& device,
                                            const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<ExplodeEffect_Instance>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : ExplodeEffect_Instance");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> ExplodeEffect_Instance::Clone(void* arg)
{
	auto instance = make_shared<ExplodeEffect_Instance>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : ExplodeEffect_Instance");
		return nullptr;
	}

	return instance;
}
