#include "pch.h"
#include "Em3000Body.h"

#include <Game.h>
#include <SpdLogger.h>

#include <Shader.h>
#include <SphereCollider.h>

#include "Bullet.h"
#include "Em3000.h"
#include "Entity.h"
#include "HowitzerBullet.h"

Em3000Body::Em3000Body() : PartObject{} {}
Em3000Body::Em3000Body(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: PartObject{device, context} {}
Em3000Body::Em3000Body(const Em3000Body& rhs)
	: PartObject{ rhs } {}

const TRANSFORM_FRAME& Em3000Body::Get_ModelTransform() const
{
	return m_Model->Get_RootTransformVelocity(m_RootBoneIndex);
}

void Em3000Body::Set_Animation(uint32 animIndex, Float blendDuration, Bool isLoop)
{
	m_Model->Set_Animation(animIndex, blendDuration);
	m_Model->Set_AnimLoop(isLoop);
}

uint32 Em3000Body::Get_CurrentAnimationIndex() const
{
	return m_Model->Get_AnimationIndex();
}

uint32 Em3000Body::Get_NextAnimationIndex() const
{
	return m_Model->Get_NextAnimationIndex();
}

Float Em3000Body::Get_AnimationProgress() const
{
	return m_Model->Get_AnimationProgress();
}

Bool Em3000Body::Is_AnimationFinished() const
{
	return m_Model->Is_AnimationFinished();
}

HRESULT Em3000Body::Initialize_Prototype()
{
	m_LayerMask.Set_Layer(L"Monster");
	m_TagMask.Set_Tag({ L"Monster" });

	return PartObject::Initialize_Prototype();
}

HRESULT Em3000Body::Initialize(void* arg)
{
	if (FAILED(PartObject::Initialize(arg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed To Ready_Components : Em3000Body");
		return E_FAIL;
	}

	if (FAILED(Ready_AnimationNotify()))
	{
		LOG_ERROR(L"Failed To Ready_AnimationNotify : Em3000Body");
		return E_FAIL;
	}

	m_RootBoneIndex = m_Model->Get_BoneIndexByName("em3000");

	if (m_RootBoneIndex == -1)
	{
		LOG_ERROR(L"Failed to Find Em3000 Root Bone");
		return E_FAIL;
	}

	m_Model->Set_LocalRootNode(m_RootBoneIndex);

	if (FAILED(Ready_Ports(2.f, 2.f)))
		return E_FAIL;

	return S_OK;
}

HRESULT Em3000Body::Begin()
{
	return S_OK;
}

void Em3000Body::On_Destroy()
{
	PartObject::On_Destroy();
}

void Em3000Body::Priority_Update(Float timeDelta)
{
	
}

void Em3000Body::Update(Float timeDelta)
{
	Float actualTimeDelta = timeDelta;
	if (auto entity = static_pointer_cast<Entity>(m_Owner.lock())) {
		if (entity->Get_LagDuration() > 0.f)
			actualTimeDelta *= 0.05f;
	}
	m_Model->Update_ModelAnimation(actualTimeDelta);
}

void Em3000Body::Late_Update(Float timeDelta)
{
	m_Transform->Update_WorldMatrix();
	Update_CombineWorldMatrix(*m_Transform->Get_WorldMatrixPtr());
	m_HitBox->Update(m_CombinedWorldMatrix);


	Fire_Projectile(timeDelta);
}

void Em3000Body::Fixed_Update(Float fixedDelta)
{
	
}

HRESULT Em3000Body::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t numMeshes = m_Model->Get_NumMeshes();
	for (uint32 i = 0; i < numMeshes; ++i)
	{
		m_Model->Bind_Material(m_Shader, DiffuseMap, i, 1, 0);
		m_Model->Bind_Material(m_Shader, NormalMap, i, 6, 0);
		m_Model->Bind_BoneMatrices(m_Shader, BoneMatrices, i);

		if (FAILED(m_Shader->Begin(1)))
			return E_FAIL;

		m_Model->Render(i);
	}

	return S_OK;
}

void Em3000Body::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

void Em3000Body::OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}

void Em3000Body::OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}

void Em3000Body::OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}

HRESULT Em3000Body::Bind_ShaderResources()
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

HRESULT Em3000Body::Ready_Components()
{
	Shader::SHADER_DESC shaderDesc{ VTXANIMMESH::Tag,  VTXANIMMESH::Elements, VTXANIMMESH::numElements };
	m_Shader = Add_Component<Shader>(ETOI(LEVEL::STATIC), &shaderDesc);
	if (nullptr == m_Shader)
		return E_FAIL;

	Model::MODEL_DESC modelDesc{ L"em3000" };
	m_Model = Add_Component<Model>(ETOI(LEVEL::STATIC), &modelDesc);
	if (nullptr == m_Model)
		return E_FAIL;

	SphereCollider::SPHERE_COLLIDER_DESC sphereDesc{};
	sphereDesc.radius = 3.f;
	sphereDesc.offset = Vector3::UnitY;
	m_HitBox = Add_Component<SphereCollider>(ETOI(LEVEL::STATIC), &sphereDesc);
	if (nullptr == m_HitBox)
		return E_FAIL;

	return S_OK;
}

HRESULT Em3000Body::Ready_AnimationNotify()
{
	using em3000State = Em3000::EM3000_STATE;
	using notify = AnimationTracker::ANIMATION_NOTIFY;
	
	m_Model->Add_AnimNotify(ETOI(em3000State::DANMAK_START_3), { 
		notify{L"Howitzer1", 1.f, [this]() { Fire_Howitzer(); }},
		notify{L"Howitzer2", 2.f, [this]() { Fire_Howitzer(); }}
	});

	return S_OK;
}

HRESULT Em3000Body::Ready_Ports(Float radius, Float muzzleOffset)
{
	uint32 numPorts{ 10 };
	Float angle = 36.f;

	Float startAngleDeg = -((numPorts - 1) / 2.0f) * angle;
	for (uint32 i = 0; i < numPorts; ++i)
	{
		Float currentAngleRad = XMConvertToRadians(startAngleDeg + i * angle);
		EM3000_PROJECTILE_PORT data;

		Vector3 dirVec = Vector3{ sinf(currentAngleRad), 0.0f, cosf(currentAngleRad) };
		data.direction = dirVec;
		data.localPosition = dirVec * radius;
		data.muzzlePosition = data.localPosition + (dirVec * muzzleOffset);
		m_Ports.push_back(data);
	}

	return S_OK;
}

void Em3000Body::Fire_Bullet()
{
	m_IsFiring = true;
	m_PortFireAccTime = 0.f;
	m_PermanentCount = 0;
	m_ModelMatrixAcc = Matrix::Identity;
}

void Em3000Body::Stop_Bullet()
{
	m_IsFiring = false;
	m_PortFireAccTime = 0.f;
	m_PermanentCount = 0;
	m_ModelMatrixAcc = Matrix::Identity;
}

void Em3000Body::Fire_Howitzer()
{
	Matrix worldMatrix = m_CombinedWorldMatrix;

	for (const auto& port : m_Ports)
	{
		Vector3 muzzleWorldPos = Vector3::Transform(port.muzzlePosition, worldMatrix);
		Vector3 muzzleWorldDir = Vector3::TransformNormal(port.direction, worldMatrix);
		muzzleWorldDir.Normalize();

		HowitzerBullet::HOWITZER_BULLET_DESC howitzerDesc{};
		howitzerDesc.initialPosition = muzzleWorldPos;
		howitzerDesc.direction = muzzleWorldDir;
		howitzerDesc.speed = 3.f;
		howitzerDesc.maxDistance = 50.f;
		howitzerDesc.resourceTag = L"candy";
		howitzerDesc.targetLayer = L"Player";
		howitzerDesc.damage = 30.f;
		howitzerDesc.isPermanent = true;
		howitzerDesc.useCurvedFlight = true;
		howitzerDesc.gravityStrength = 5.f;
		howitzerDesc.targetY = 20.f; // 지면 높이

		GAME_INSTANCE->Instantiate<HowitzerBullet>(
			L"HowitzerBullet",
			GAME_INSTANCE->Get_TargetLevelIndex(),
			&howitzerDesc);
	}
}

void Em3000Body::Fire_Projectile(Float timeDelta)
{
	if (false == m_IsFiring)
	{
		return;
	}

	m_PortFireAccTime += timeDelta;
	if (m_PortFireAccTime < m_PortFireRate)
		return;

	m_PortFireAccTime = 0.f;
	Matrix worldMatrix = m_CombinedWorldMatrix;

	Bool isPermanent{ false };
	if (m_PermanentCount == 3)
	{
		isPermanent = true;
		m_PermanentCount = 0;
	}
	else
		isPermanent = false;
	
	for (const auto& port : m_Ports)
	{
		Vector3 muzzleWorldPos = Vector3::Transform(port.muzzlePosition, worldMatrix);
		Vector3 muzzleWorldDir = Vector3::TransformNormal(port.direction, worldMatrix);

		muzzleWorldDir.Normalize();
		// [곡사포 Bullet 구조체 할당]
		Bullet::BULLET_DESC bulletDesc{};
		bulletDesc.initialPosition = muzzleWorldPos + Vector3{ 0.f, 0.75f, 0.f };
		bulletDesc.direction = muzzleWorldDir;
		bulletDesc.speed = 10.f;           // 전방으로 나아가는 힘
		bulletDesc.maxDistance = 18.5f;          // 수명 또는 사거리
		bulletDesc.resourceTag = L"candy";
		bulletDesc.targetLayer = L"Player";      // 맞출 대상
		bulletDesc.damage = 10.f;
		bulletDesc.isPermanent = isPermanent;

		auto bullet = GAME_INSTANCE->Instantiate<Bullet>(L"Bullet", GAME_INSTANCE->Get_TargetLevelIndex(), &bulletDesc);
		bullet->Get_Transform()->Set_Scale(Vector3{ 0.5f, 0.5f, 0.5f });
	}
	++m_PermanentCount;
}

Shared<Em3000Body> Em3000Body::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<Em3000Body>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Em3000Body");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> Em3000Body::Clone(void* arg)
{
	auto instance = make_shared<Em3000Body>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : Em3000Body");
		return nullptr;
	}

	return instance;
}

