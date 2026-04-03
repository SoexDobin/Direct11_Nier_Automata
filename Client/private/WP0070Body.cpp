#include "pch.h"
#include "WP0070Body.h"

#include <SpdLogger.h>

#include "Game.h"
#include "Model.h"
#include "OBBCollider.h"

WP0070Body::WP0070Body() : Pl0000Parts{} {}
WP0070Body::WP0070Body(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Pl0000Parts{ device, context } {
}
WP0070Body::WP0070Body(const WP0070Body& rhs)
	: Pl0000Parts{ rhs } {
}

HRESULT WP0070Body::Initialize_Prototype()
{
	m_LayerMask.Set_Layer(L"PlayerWeapon");
	m_TagMask.Set_Tag({ L"PlayerWeapon" });

	return Pl0000Parts::Initialize_Prototype();
}

HRESULT WP0070Body::Initialize(void* arg)
{
	if (FAILED(Pl0000Parts::Initialize(arg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed To Ready_Components : WP0070Body");
		return E_FAIL;
	}

	m_RootBoneIndex = m_Model->Get_BoneIndexByName("wp0070");
	m_WeaponBoneIndex = m_Model->Get_BoneIndexByName("bone0");
	if (m_RootBoneIndex == -1)
	{
		LOG_ERROR(L"Failed to Find WP0070 Root Bone");
		return E_FAIL;
	}
	
	m_Model->Set_LocalRootNode(m_RootBoneIndex);

	return S_OK;
}

void WP0070Body::On_Destroy()
{
	PartObject::On_Destroy();
}

void WP0070Body::Priority_Update(Float timeDelta)
{

}

void WP0070Body::Update(Float timeDelta)
{
	m_Model->Update_ModelAnimation(timeDelta);

	if (m_IsSheathing == false)
	{
		TRANSFORM_FRAME rootVelocity = m_Model->Get_RootTransformVelocity(m_WeaponBoneIndex);

		Vector3 localPos = m_Transform->Get_Position();
		localPos += Vector3{ rootVelocity.position.x, rootVelocity.position.y, rootVelocity.position.z };
		m_Transform->Set_Position(localPos);
	}
}

void WP0070Body::Late_Update(Float timeDelta)
{
	Update_CombineWorldMatrix(*m_Transform->Get_WorldMatrixPtr());
	
	Matrix boneMatrix = m_Model->Get_BoneMatrix(m_WeaponBoneIndex);
	m_AttackCollider->Update(boneMatrix * m_CombinedWorldMatrix);
	
}

void WP0070Body::Fixed_Update(Float fixedDelta)
{

}

HRESULT WP0070Body::Render()
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

void WP0070Body::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

void WP0070Body::OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}

void WP0070Body::OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}

void WP0070Body::OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}

void WP0070Body::Set_Sheathing(const Matrix& sheathMatrix)
{
	if (m_IsSheathing) return;

	//m_AttackCollider->Set_Active(false);
	m_Model->Set_Animation(ETOI(WP0070_STATE::SHEATHE_LIGHT), 0.05f);
	m_Transform->Set_WorldMatrix(sheathMatrix);
	m_IsSheathing = true;
}

void WP0070Body::DrawWP0070()
{
	if (m_IsSheathing == false) return;

	//m_AttackCollider->Set_Active(true);
	m_Transform->Set_WorldMatrix(Matrix::Identity);
	m_IsSheathing = false;
}

void WP0070Body::Set_Animation(uint32 animIndex, Float blendDuration, Bool isLoop)
{
	m_Transform->Set_WorldMatrix(Matrix::Identity);
	Pl0000Parts::Set_Animation(animIndex, blendDuration, isLoop);
}

HRESULT WP0070Body::Bind_ShaderResources()
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

HRESULT WP0070Body::Ready_Components()
{
	Shader::SHADER_DESC shaderDesc{ VTXANIMMESH::Tag,  VTXANIMMESH::Elements, VTXANIMMESH::numElements };
	m_Shader = Add_Component<Shader>(ETOI(LEVEL::STATIC), &shaderDesc);
	if (nullptr == m_Shader)
		return E_FAIL;

	Model::MODEL_DESC modelDesc{ L"wp0070" };
	m_Model = Add_Component<Model>(ETOI(LEVEL::STATIC), &modelDesc);
	if (nullptr == m_Model)
		return E_FAIL;

	OBBCollider::OBB_COLLIDER_DESC colDesc{};
	colDesc.extents = Vector3{ 0.075f, 0.1f, 0.7f }; 
	colDesc.rotation = Vector3::Zero;
	colDesc.offset = Vector3{ 0.f, 0.f, -0.55f };
	m_AttackCollider = Add_Component<OBBCollider>(ETOI(LEVEL::STATIC), &colDesc);
	if (nullptr == m_AttackCollider)
		return E_FAIL;
	
	return S_OK;
}

Shared<WP0070Body> WP0070Body::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<WP0070Body>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : WP0070Body");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> WP0070Body::Clone(void* arg)
{
	auto instance = make_shared<WP0070Body>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : WP0070Body");
		return nullptr;
	}

	return instance;
}


