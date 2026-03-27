#include "pch.h"
#include "Pl0000Body.h"

#include <SpdLogger.h>

#include "Game.h"

#include "Model.h"
#include "Pl0000.h"

Pl0000Body::Pl0000Body() : Pl0000Parts{} {}
Pl0000Body::Pl0000Body(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Pl0000Parts{device, context} {}
Pl0000Body::Pl0000Body(const Pl0000Body& rhs)
	: Pl0000Parts{rhs} {}

HRESULT Pl0000Body::Initialize_Prototype()
{
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

	m_RootBoneIndex = m_Model->Get_BoneIndexByName("pl0000");
	m_Model->Set_LocalRootNode(m_RootBoneIndex);
	if (m_RootBoneIndex == -1)
	{
		LOG_ERROR(L"Failed to Find 2B Root Bone");
		return E_FAIL;
	}

	GAME_INSTANCE->Get_MainCamera()->Set_Target(shared_from_this());
	m_LastDirection = m_Transform->Get_Look();

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
	m_Model->Update_ModelAnimation(timeDelta);
	Update_Movement(timeDelta);
	m_Transform->Update_WorldMatrix();
}

void Pl0000Body::Late_Update(Float timeDelta)
{
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

void Pl0000Body::Update_Movement(Float timeDelta)
{
	Vector3 inputDir = Vector3::Zero;
	if (GAME_INSTANCE->Get_DIKeyState(DIKEYBOARD_W) & 0x80) inputDir.z += 1.f;
	if (GAME_INSTANCE->Get_DIKeyState(DIKEYBOARD_S) & 0x80) inputDir.z -= 1.f;
	if (GAME_INSTANCE->Get_DIKeyState(DIKEYBOARD_A) & 0x80) inputDir.x -= 1.f;
	if (GAME_INSTANCE->Get_DIKeyState(DIKEYBOARD_D) & 0x80) inputDir.x += 1.f;

	if (inputDir.LengthSquared() > 0.f)
	{
		inputDir.Normalize();

		auto camera = GAME_INSTANCE->Get_MainCamera();
		Vector3 camLook = camera->Get_Transform()->Get_Look();
		Vector3 camRight = camera->Get_Transform()->Get_Right();

		camLook.y = 0.f; camLook.Normalize();
		camRight.y = 0.f; camRight.Normalize();

		Vector3 targetDir = (camLook * inputDir.z) + (camRight * inputDir.x);
		targetDir.Normalize();

		// 3. 캐릭터의 현재 회전값을 목표 방향으로 부드럽게 회전 (Slerp 활용)
		float targetYaw = atan2f(targetDir.x, targetDir.z); // XZ 평면에서의 회전 각도(Yaw)
		Quaternion targetQuat = Quaternion::CreateFromYawPitchRoll(targetYaw, 0.f, 0.f);

		// turnSpeed가 작을수록 크게 둥글게 돌고, 클수록 좁게 휙 돕니다. (원하는 조작감에 맞춰 수정)
		float turnSpeed = 8.0f * timeDelta;
		Quaternion currentQuat = m_Transform->Get_Quaternion();
		Quaternion nextQuat = Quaternion::Slerp(currentQuat, targetQuat, turnSpeed);

		// 회전 적용
		m_Transform->Set_Rotation(nextQuat);

		// 4. ★핵심: 이동 방향(dir)은 targetDir이 아니라 방금 회전한 캐릭터의 현재 Look 방향!
		Vector3 currentLook = m_Transform->Get_Look();
		currentLook.y = 0.f;
		currentLook.Normalize();

		m_LastDirection = currentLook;

		// (옵션) 기존 루트 모션 방식 사용 시
		TRANSFORM_FRAME transformFrame = m_Model->Get_RootTransformVelocity(m_RootBoneIndex);
		Vector3 worldMoveDelta = Vector3::Transform(transformFrame.position * -1.f, nextQuat);
		m_Transform->Set_Position(m_Transform->Get_Position() + worldMoveDelta * timeDelta);
	}
}

HRESULT Pl0000Body::Bind_ShaderResources()
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


