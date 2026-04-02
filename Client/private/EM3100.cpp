#include "pch.h"
#include "EM3100.h"

#include <Game.h>

#include "Shader.h"
#include "Model.h"
#include "AABBCollider.h"
#include "SphereCollider.h"
#include "SpdLogger.h"
#include "Random_Helper.h"

namespace Engine
{
	class Model;
}

EM3100::EM3100() : Monster{} {}
EM3100::EM3100(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Monster{device, context} {}
EM3100::EM3100(const EM3100& rhs)
	: Monster{rhs} {}

HRESULT EM3100::Initialize_Prototype()
{
	return Monster::Initialize_Prototype();
}

HRESULT EM3100::Initialize(void* arg)
{
	if (FAILED(Monster::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize em3100");
		return E_FAIL;
	}

	if (FAILED(Ready_PartObjects()))
	{
		LOG_ERROR(L"Failed to Ready PartObjects em3100 ");
		return E_FAIL;
	}

	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed to Ready Components em3100 ");
		return E_FAIL;
	}

	int32 rand = Helper::Random_Int(0, 3);
	m_Model->Set_AnimationIndex(rand);
	m_Model->Set_AnimLoop(true);

	return S_OK;
}

void EM3100::Priority_Update(Float timeDelta)
{
	Monster::Priority_Update(timeDelta);
}

void EM3100::Update(Float timeDelta)
{
	m_Model->Update_ModelAnimation(timeDelta);
	m_Transform->Update_WorldMatrix();
	m_HitBox->Update(m_Transform->Get_WorldMatrix());
	m_InteractionZone->Update(m_Transform->Get_WorldMatrix());
}

void EM3100::Late_Update(Float timeDelta)
{
	
}

void EM3100::Fixed_Update(Float fixedDelta)
{
	
}

HRESULT EM3100::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t numMeshes = m_Model->Get_NumMeshes();
	for (uint32 i = 0; i < numMeshes; ++i)
	{
		m_Model->Bind_Material(m_Shader, DiffuseMap, i, 1, 0);
		m_Model->Bind_BoneMatrices(m_Shader, BoneMatrices, i);

		if (FAILED(m_Shader->Begin(0)))
			return E_FAIL;

		m_Model->Render(i);
	}

	return S_OK;
}

void EM3100::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

void EM3100::OnCollisionEnter(const Shared<GameObject>& collision)
{
	auto a = collision;
}

void EM3100::OnCollisionStay(const Shared<GameObject>& collision)
{
	
}

void EM3100::OnCollisionExit(const Shared<GameObject>& collision)
{
	
}

HRESULT EM3100::Bind_ShaderResources()
{
	if (FAILED(m_Transform->Bind_ShaderResource(m_Shader, WorldMatrix)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_Shader, ViewMatrix, D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_Shader, ProjMatrix, D3DTS::PROJ)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_CameraPosition(m_Shader, CameraPosition)))
		return E_FAIL;

	const LIGHT_DESC* lightDesc = GAME_INSTANCE->Get_LightDesc(0);
	if (nullptr == lightDesc)
		return E_FAIL;

	if (FAILED(m_Shader->Bind_RawValue(DirectionLight, &lightDesc->direction, sizeof(Float4))))
		return E_FAIL;
	if (FAILED(m_Shader->Bind_RawValue(DiffuseLight, &lightDesc->diffuse, sizeof(Float4))))
		return E_FAIL;
	if (FAILED(m_Shader->Bind_RawValue(AmbientLight, &lightDesc->ambient, sizeof(Float4))))
		return E_FAIL;
	if (FAILED(m_Shader->Bind_RawValue(SpecularLight, &lightDesc->specular, sizeof(Float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT EM3100::Ready_PartObjects()
{

	return S_OK;
}

HRESULT EM3100::Ready_Components()
{
	Shader::SHADER_DESC shaderDesc{ VTXANIMMESH::Tag,  VTXANIMMESH::Elements, VTXANIMMESH::numElements };
	m_Shader = Add_Component<Shader>(ETOI(LEVEL::STATIC), &shaderDesc);
	if (nullptr == m_Shader)
		return E_FAIL;

	Model::MODEL_DESC modelDesc{ L"em3100" };
	m_Model = Add_Component<Model>(ETOI(LEVEL::STATIC), &modelDesc);
	if (nullptr == m_Model)
		return E_FAIL;

	AABBCollider::AABB_COLLIDER_DESC aabbDesc{};
	aabbDesc.extents = Vector3{ 1.f, 1.f, 1.f };
	aabbDesc.offset = Vector3::Zero;
	m_HitBox = Add_Component<AABBCollider>(ETOI(LEVEL::STATIC), &aabbDesc);
	if (nullptr == m_HitBox)
		return E_FAIL;

	SphereCollider::SPHERE_COLLIDER_DESC sphereDesc{};
	sphereDesc.radius = 1.f;
	sphereDesc.offset = Vector3::Zero;
	m_InteractionZone = Add_Component<SphereCollider>(ETOI(LEVEL::STATIC), &sphereDesc);
	if (nullptr == m_InteractionZone)
		return E_FAIL;

	return S_OK;
}

Shared<EM3100> EM3100::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<EM3100>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : EM3100");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> EM3100::Clone(void* arg)
{
	auto instance = make_shared<EM3100>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : EM3100");
		return nullptr;
	}

	return instance;
}


