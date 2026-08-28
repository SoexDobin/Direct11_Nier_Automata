#include "pch.h"
#include "Pl0000Body.h"

#include <SpdLogger.h>

#include "Game.h"

#include "AABBCollider.h"
#include "Entity.h"

Pl0000Body::Pl0000Body() : Pl0000Parts{} {}
Pl0000Body::Pl0000Body(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Pl0000Parts{device, context} {}
Pl0000Body::Pl0000Body(const Pl0000Body& rhs)
	: Pl0000Parts{rhs} {}

HRESULT Pl0000Body::Initialize_Prototype()
{
	m_LayerMask.Set_Layer(L"Player");
	m_TagMask.Set_Tag({ L"Player" });

	return PartObject::Initialize_Prototype();
}

HRESULT Pl0000Body::Initialize(void* arg)
{
	if (FAILED(Pl0000Parts::Initialize(arg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed To Ready_Components : Pl0000Body");
		return E_FAIL;
	}

	if (FAILED(Ready_AnimationNotify()))
	{
		LOG_ERROR(L"Failed To Ready_AnimationNotify : Pl0000Body");
		return E_FAIL;
	}

	m_RootBoneIndex = m_Model->Get_BoneIndexByName("pl0000");
	
	if (m_RootBoneIndex == -1)
	{
		LOG_ERROR(L"Failed to Find 2B Root Bone");
		return E_FAIL;
	}

	m_Model->Set_LocalRootNode(m_RootBoneIndex);

	return S_OK;
}

void Pl0000Body::On_Destroy()
{
	PartObject::On_Destroy();
}

void Pl0000Body::Priority_Update(Float timeDelta)
{

}

void Pl0000Body::Update(Float timeDelta)
{
	Float actualTimeDelta = timeDelta;
	if (auto entity = dynamic_pointer_cast<Entity>(Get_Owner())) {
		if (entity->Get_LagDuration() > 0.f) actualTimeDelta *= 0.05f;
	}
	m_Model->Update_ModelAnimation(actualTimeDelta);
}

void Pl0000Body::Late_Update(Float timeDelta)
{
	m_Transform->Update_WorldMatrix();
	Update_CombineWorldMatrix(*m_Transform->Get_WorldMatrixPtr());
	m_HitBox->Update(m_CombinedWorldMatrix);
}

void Pl0000Body::Fixed_Update(Float fixedDelta)
{
	
}

HRESULT Pl0000Body::Render()
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

void Pl0000Body::Submit_RenderGroup()
{
	if (Is_Active())
		GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

HRESULT Pl0000Body::Bind_ShaderResources()
{
	if (FAILED(m_Shader->Bind_Matrix(WorldMatrix, &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_Shader, ViewMatrix, D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_Shader, ProjMatrix, D3DTS::PROJ)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_CameraPosition(m_Shader, CameraPosition)))
		return E_FAIL;

	return S_OK;
}

HRESULT Pl0000Body::Ready_Components()
{

	Shader::SHADER_DESC shaderDesc{ VTXANIMMESH::Tag,  VTXANIMMESH::Elements, VTXANIMMESH::numElements };
	m_Shader = Add_Component<Shader>(ETOI(LEVEL::STATIC), &shaderDesc);
	if (nullptr == m_Shader)
		return E_FAIL;

	Model::MODEL_DESC modelDesc{ L"p10000" };
	m_Model = Add_Component<Model>(ETOI(LEVEL::STATIC), &modelDesc);
	if (nullptr == m_Model)
		return E_FAIL;

	AABBCollider::AABB_COLLIDER_DESC colDesc{};
	colDesc.extents = Vector3{ 0.5f, 1.f, 0.5f };
	colDesc.offset = Vector3::UnitY;
	m_HitBox = Add_Component<AABBCollider>(ETOI(LEVEL::STATIC), &colDesc);

	return S_OK;
}

HRESULT Pl0000Body::Ready_AnimationNotify()
{

	return S_OK;
}

Shared<Pl0000Body> Pl0000Body::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<Pl0000Body>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Pl0000Body");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> Pl0000Body::Clone(void* arg)
{
	auto instance = make_shared<Pl0000Body>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : Pl0000Body");
		return nullptr;
	}

	return instance;
}


