#include "pch.h"
#include "WP3000Body.h"

#include <SpdLogger.h>

#include "Bullet.h"
#include "Game.h"
#include "Model.h"
#include "Pl0000.h"


NS_BEGIN(Client)
	WP3000Body::WP3000Body() : Pl0000Parts{} {}
WP3000Body::WP3000Body(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Pl0000Parts{ device, context } {}
WP3000Body::WP3000Body(const WP3000Body& rhs)
	: Pl0000Parts{ rhs } {}

HRESULT WP3000Body::Initialize_Prototype()
{
	return Pl0000Parts::Initialize_Prototype();
}

HRESULT WP3000Body::Initialize(void* arg)
{
	if (FAILED(Pl0000Parts::Initialize(arg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed To Ready_Components : WP3000Body");
		return E_FAIL;
	}

    // POD 모델 루트 본 설정 (있는 경우)
	m_RootBoneIndex = m_Model->Get_BoneIndexByName("wp3000");

	if (m_RootBoneIndex != -1)
	{
		m_Model->Set_LocalRootNode(m_RootBoneIndex);
	}

    m_Model->Set_Animation(0, 0.f);

	return S_OK;
}

void WP3000Body::On_Destroy()
{
	PartObject::On_Destroy();
}

void WP3000Body::Priority_Update(Float timeDelta)
{
}

void WP3000Body::Update(Float timeDelta)
{
	uint32 animIndex = Get_CurrentAnimationIndex();

	if (GAME_INSTANCE->Get_DIKeyState(DIK_LSHIFT) & 0x80)
	{
		if (animIndex == ETOI(POD_STATE::IDLE))
		{
			m_PodState = POD_STATE::SHOOT_START;
			Set_Animation(ETOI(POD_STATE::SHOOT_START), 0.2f, false);
		}
		else if (animIndex == ETOI(POD_STATE::SHOOT_START) &&
			Is_AnimationFinished())
		{
			m_PodState = POD_STATE::SHOOT_LOOP;
			Set_Animation(ETOI(POD_STATE::SHOOT_LOOP), 0.2f, false);
		}
	}
	else
	{
		if (animIndex == ETOI(POD_STATE::SHOOT_LOOP))
		{
			m_PodState = POD_STATE::SHOOT_END;
			Set_Animation(ETOI(POD_STATE::SHOOT_END), 0.2f, false);
		}
		else if (animIndex == ETOI(POD_STATE::SHOOT_END) &&
			Is_AnimationFinished())
		{
			m_PodState = POD_STATE::IDLE;
			Set_Animation(ETOI(POD_STATE::IDLE), 0.2f, false);
		}
	}

	if (animIndex == ETOI(POD_STATE::SHOOT_LOOP))
		Pod_Fire(timeDelta);

	m_Model->Update_ModelAnimation(timeDelta);
}

void WP3000Body::Late_Update(Float timeDelta)
{
	if (m_Pl0000.expired()) return;
	if (m_Pl0000Body.expired()) return;

	Float hoverOffset = 0.f;
	if (m_PodState == POD_STATE::IDLE)
	{
		m_HoverTime += timeDelta * m_HoverSpeed;
		hoverOffset = std::sin(m_HoverTime) * m_HoverAmplitude;
	}
	
	Vector3 targetOffset = m_TargetOffset;
	targetOffset.y += hoverOffset;
	m_Transform->Set_Position(targetOffset);
	
	m_Transform->Update_WorldMatrix();
	
	Update_CombineWorldMatrix(*m_Transform->Get_WorldMatrixPtr());
	
	if (m_PodState != POD_STATE::IDLE)
	{
		const Shared<GameObject>& camera = GAME_INSTANCE->Get_MainCamera();
		if (camera != nullptr)
		{
			Vector3 scale, position;
			Quaternion rot;

			// 부모와 결합 된 결과물(m_CombinedWorldMatrix)에서 최종 World Position과 Scale만 축출
			m_CombinedWorldMatrix.Decompose(scale, rot, position);
			
			Quaternion cameraRot = camera->Get_Transform()->Get_Quaternion();
			m_CombinedWorldMatrix = Matrix::CreateScale(scale)
				* Matrix::CreateFromQuaternion(cameraRot)
				* Matrix::CreateTranslation(position);
		}
	}

}

void WP3000Body::Fixed_Update(Float fixedDelta)
{
}

HRESULT WP3000Body::Render()
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

void WP3000Body::Submit_RenderGroup()
{
	if (Is_Active())
		GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

HRESULT WP3000Body::Bind_ShaderResources()
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

HRESULT WP3000Body::Ready_Components()
{
	Shader::SHADER_DESC shaderDesc{ VTXANIMMESH::Tag,  VTXANIMMESH::Elements, VTXANIMMESH::numElements };
	m_Shader = Add_Component<Shader>(ETOI(LEVEL::STATIC), &shaderDesc);
	if (nullptr == m_Shader)
		return E_FAIL;

	Model::MODEL_DESC modelDesc{ L"wp3000" };
	m_Model = Add_Component<Model>(ETOI(LEVEL::STATIC), &modelDesc);
	if (nullptr == m_Model)
		return E_FAIL;

	return S_OK;
}

void WP3000Body::Pod_Fire(Float timeDelta)
{
	m_FireRateTimer += timeDelta;
	// Shift 키를 누르고 있을 때

	if (GAME_INSTANCE->Get_DIKeyState(DIK_LSHIFT) & 0x8000)
	{
		if (m_FireRateTimer >= m_FireRate)
		{
			Vector3 worldScale{}, worldPosition{};
			Quaternion worldRot{};
			m_CombinedWorldMatrix.Decompose(worldScale, worldRot, worldPosition);

			m_FireRateTimer = 0.f;
			// 총알 매개변수 세팅
			Entity::DAMAGE_INFO dmgInfo{};
			dmgInfo.attackType = ATK_TYPE::POD;
			dmgInfo.attackerPos = m_Transform->Get_Position();
			dmgInfo.damage = 0.f;
			dmgInfo.groggyWeight = 0;
			dmgInfo.knockbackForce = 0.2f;

			Bullet::BULLET_DESC desc{};
			desc.damageInfo = dmgInfo;
			desc.resourceTag = L"candy";
			desc.targetLayer = L"Monster";
			desc.isPermanent = false;
			desc.speed = 30.0f; // 탄속
			desc.maxDistance = 100.f; // 50미터 쯤 날아가면 소멸
			desc.initialPosition = worldPosition + Vector3{0.f, 0.25f, 0.f };
			desc.direction = m_CombinedWorldMatrix.Backward() + Vector3{ 0.f, 0.25f, 0.f };

			GAME_INSTANCE->Instantiate<Bullet>(L"Bullet", ETOI(LEVEL::GAMEPLAY), &desc);
			GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_14);
			GAME_INSTANCE->PlaySoundFXOnce(L"Wp3000_Shot", SOUNDCHANNEL::CHANNEL_14, 0.4f);
		}
	}
}

Shared<WP3000Body> WP3000Body::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<WP3000Body>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : WP3000Body");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> WP3000Body::Clone(void* arg)
{
	auto instance = make_shared<WP3000Body>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : WP3000Body");
		return nullptr;
	}

	return instance;
}

NS_END
