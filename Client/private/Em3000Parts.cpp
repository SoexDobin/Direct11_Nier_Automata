#include "pch.h"
#include "Em3000Parts.h"

#include <Game.h>

#include "Model.h"
#include "SpdLogger.h"


Em3000Parts::Em3000Parts() : PartObject{} {}
Em3000Parts::Em3000Parts(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: PartObject{device, context} {}
Em3000Parts::Em3000Parts(const Em3000Parts& rhs)
	: PartObject{rhs} {}

HRESULT Em3000Parts::Initialize_Prototype()
{
	m_LayerMask.Set_Layer(L"Monster");
	m_TagMask.Set_Tag({ L"Monster" });

	return PartObject::Initialize_Prototype();
}

HRESULT Em3000Parts::Initialize(void* arg)
{
	if (nullptr == arg)
	{
		LOG_ERROR(L"Failed to Initialize Em3000Part");
		return E_FAIL;
	}

	auto desc = static_cast<tagEm3000PartsDesc*>(arg);
	if (false == desc->bodyModel.expired())
	{
		m_Model = desc->bodyModel.lock();

		if (nullptr == desc->targetBoneName)
		{
			m_BoneName = desc->targetBoneName;
			m_TargetBoneIndex = m_BodyModel.lock()->Get_BoneIndexByName(m_BoneName);
		}
	} else
	{
		LOG_ERROR(L"Failed to get BodyModel Em3000Parts");
		return E_FAIL;
	}

	if (FAILED(Ready_Components(desc->shaderDesc, desc->modelResourceTag)))
	{
		LOG_ERROR(L"Failed to Ready_Components BodyModel Em3000Parts");
		return E_FAIL;
	}

	return PartObject::Initialize(arg);
}

void Em3000Parts::Set_Animation(uint32 animIndex, Float blendDuration, Bool isLoop)
{
	m_Model->Set_Animation(animIndex, blendDuration);
	m_Model->Set_AnimLoop(isLoop);
}

uint32 Em3000Parts::Get_CurrentAnimationIndex() const
{
	return m_Model->Get_AnimationIndex();
}

uint32 Em3000Parts::Get_NextAnimationIndex() const
{
	return m_Model->Get_NextAnimationIndex();
}

Float Em3000Parts::Get_AnimationProgress() const
{
	return m_Model->Get_AnimationProgress();
}

Bool Em3000Parts::Is_AnimationFinished() const
{
	return m_Model->Is_AnimationFinished();
}

void Em3000Parts::Update(Float timeDelta)
{
	
}

void Em3000Parts::Late_Update(Float timeDelta)
{
	if (m_TargetBoneIndex == -1 || m_BodyModel.expired()) return;

	m_Transform->Update_WorldMatrix();

	Matrix boneMatrix = m_BodyModel.lock()->Get_BoneMatrix(m_TargetBoneIndex);

	m_CombinedWorldMatrix = m_Transform->Get_WorldMatrix() * boneMatrix * (*m_ParentMatrix);
}

HRESULT Em3000Parts::Render()
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

void Em3000Parts::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

HRESULT Em3000Parts::Bind_ShaderResources()
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

HRESULT Em3000Parts::Ready_Components(Shader::SHADER_DESC shaderDesc, const wstring& modelResourceTag)
{
	m_Shader = Add_Component<Shader>(ETOI(LEVEL::STATIC), &shaderDesc);
	if (nullptr == m_Shader) return E_FAIL;

	Model::MODEL_DESC modelDesc{ modelResourceTag };
	m_Model = Add_Component<Model>(ETOI(LEVEL::STATIC), &modelDesc);
	if (nullptr == m_Model) return E_FAIL;

	return S_OK;
}
