#include "pch.h"
#include "Em3100Body.h"

#include <Random_Helper.h>

#include "AABBCollider.h"
#include <SpdLogger.h>

#include "Game.h"
#include "Model.h"


Em3100Body::Em3100Body() : PartObject{} {}
Em3100Body::Em3100Body(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: PartObject{ device, context } {
}
Em3100Body::Em3100Body(const Em3100Body& rhs)
	: PartObject{ rhs } {
}

HRESULT Em3100Body::Initialize_Prototype()
{
	m_LayerMask.Set_Layer(L"Monster");
	m_TagMask.Set_Tag({ L"Monster" });

	return PartObject::Initialize_Prototype();
}

HRESULT Em3100Body::Initialize(void* arg)
{
	if (FAILED(PartObject::Initialize(arg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed To Ready_Components : EM3100Body");
		return E_FAIL;
	}

	int32 rand = Helper::Random_Int(0, 3);
	m_Model->Set_AnimationIndex(rand);
	m_Model->Set_AnimLoop(true);

	return S_OK;
}

void Em3100Body::On_Destroy()
{
	PartObject::On_Destroy();
}

void Em3100Body::Priority_Update(Float timeDelta)
{
}

void Em3100Body::Update(Float timeDelta)
{
	m_Model->Update_ModelAnimation(timeDelta);
}

void Em3100Body::Late_Update(Float timeDelta)
{
	m_Transform->Update_WorldMatrix();
	Update_CombineWorldMatrix(*m_Transform->Get_WorldMatrixPtr());
	m_HitBox->Update(m_CombinedWorldMatrix);
}

void Em3100Body::Fixed_Update(Float fixedDelta)
{

}

HRESULT Em3100Body::Render()
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

void Em3100Body::Submit_RenderGroup()
{
	if (Is_Active())
		GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

HRESULT Em3100Body::Bind_ShaderResources()
{
	if (FAILED(m_Shader->Bind_Matrix(WorldMatrix, &m_CombinedWorldMatrix)))
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

HRESULT Em3100Body::Ready_Components()
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
	aabbDesc.offset = Vector3{ 0.f, 1.f, 0.f };;
	m_HitBox = Add_Component<AABBCollider>(ETOI(LEVEL::STATIC), &aabbDesc);
	if (nullptr == m_HitBox)
		return E_FAIL;

	return S_OK;
}

Shared<Em3100Body> Em3100Body::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<Em3100Body>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Em3100Body");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> Em3100Body::Clone(void* arg)
{
	auto instance = make_shared<Em3100Body>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : Em3100Body");
		return nullptr;
	}

	return instance;
}
