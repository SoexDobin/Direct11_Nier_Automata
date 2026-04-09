#include "pch.h"
#include "Em0010Body.h"

#include <AABBCollider.h>
#include <Game.h>
#include <Model.h>
#include <Shader.h>
#include <SpdLogger.h>

Em0010Body::Em0010Body() : PartObject{} {}
Em0010Body::Em0010Body(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: PartObject{ device, context}{}
Em0010Body::Em0010Body(const Em0010Body& rhs)
	: PartObject{rhs} {}

HRESULT Em0010Body::Initialize_Prototype()
{
	m_LayerMask.Set_Layer(L"Monster");
	m_TagMask.Set_Tag({ L"Monster" });

	return PartObject::Initialize_Prototype();
}

HRESULT Em0010Body::Initialize(void* arg)
{
	if (FAILED(PartObject::Initialize(arg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed To Ready_Components : Em0010Body");
		return E_FAIL;
	}

	if (FAILED(Ready_AnimationNotify()))
	{
		LOG_ERROR(L"Failed To Ready_AnimationNotify : Em0010Body");
		return E_FAIL;
	}

	m_RootBoneIndex = m_Model->Get_BoneIndexByName("em0010");

	if (m_RootBoneIndex == -1)
	{
		LOG_ERROR(L"Failed to Find Em0010 Root Bone");
		return E_FAIL;
	}

	m_Model->Set_LocalRootNode(m_RootBoneIndex);

	return S_OK;
}

void Em0010Body::On_Destroy()
{
	PartObject::On_Destroy();
}

const TRANSFORM_FRAME& Em0010Body::Get_ModelTransform() const
{
	return m_Model->Get_RootTransformVelocity(m_RootBoneIndex);
}

void Em0010Body::Set_Animation(uint32 animIndex, Float blendDuration, Bool isLoop)
{
	m_Model->Set_Animation(animIndex, blendDuration);
	m_Model->Set_AnimLoop(isLoop);
}

uint32 Em0010Body::Get_CurrentAnimationIndex() const
{
	return m_Model->Get_AnimationIndex();
}

uint32 Em0010Body::Get_NextAnimationIndex() const
{
	return m_Model->Get_NextAnimationIndex();
}

Float Em0010Body::Get_AnimationProgress() const
{
	return m_Model->Get_AnimationProgress();
}

Bool Em0010Body::Is_AnimationFinished() const
{
	return m_Model->Is_AnimationFinished();
}

void Em0010Body::Priority_Update(Float timeDelta)
{
	
}

void Em0010Body::Update(Float timeDelta)
{
	m_Model->Update_ModelAnimation(timeDelta);
}

void Em0010Body::Late_Update(Float timeDelta)
{
	m_Transform->Update_WorldMatrix();
	Update_CombineWorldMatrix(*m_Transform->Get_WorldMatrixPtr());
	m_HitBox->Update(m_CombinedWorldMatrix);
}

void Em0010Body::Fixed_Update(Float fixedDelta)
{
	
}

HRESULT Em0010Body::Render()
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

void Em0010Body::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

HRESULT Em0010Body::Bind_ShaderResources()
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

HRESULT Em0010Body::Ready_Components()
{
	Shader::SHADER_DESC shaderDesc{ VTXANIMMESH::Tag,  VTXANIMMESH::Elements, VTXANIMMESH::numElements };
	m_Shader = Add_Component<Shader>(ETOI(LEVEL::STATIC), &shaderDesc);
	if (nullptr == m_Shader)
		return E_FAIL;

	Model::MODEL_DESC modelDesc{ L"em0010" };
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

HRESULT Em0010Body::Ready_AnimationNotify()
{
	return S_OK;
}

Shared<Em0010Body> Em0010Body::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<Em0010Body>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Em0010Body");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> Em0010Body::Clone(void* arg)
{
	auto instance = make_shared<Em0010Body>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : Em0010Body");
		return nullptr;
	}

	return instance;
}


