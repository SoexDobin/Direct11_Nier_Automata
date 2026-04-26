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
#include "MonsterAOE.h"
#include "MonsterShockWave.h"

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
	m_CenterAoe = static_pointer_cast<MonsterAOE>(m_Owner.lock()->Find_PartObject(L"Em3000CenterAoe"));
	if (m_CenterAoe.expired())
		return E_FAIL;
	m_CenterAoe.lock()->DeActive_Attack();


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
	m_Model->Update_ModelAnimation(timeDelta * m_AnimSpeed);
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
	sphereDesc.radius = 3.5f;
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

	auto activeCenterAoe = [this]() { m_CenterAoe.lock()->Active_Attack(); };
	auto deActiveCenterAoe = [this]() { m_CenterAoe.lock()->DeActive_Attack(); };

	uint32 levIndex = GAME_INSTANCE->Get_TargetLevelIndex();

	m_Model->Add_AnimNotify(ETOI(em3000State::DANMAK_START_1), {
		notify{L"Stop_Sound", 0.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_26); }},
		notify{L"Play_Sound", 0.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Skirt_Open", SOUNDCHANNEL::CHANNEL_26, 0.5f); }},
		});
	m_Model->Add_AnimNotify(ETOI(em3000State::DANMAK_START_2), {
		notify{L"Stop_Sound", 0.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_26); }},
		notify{L"Play_Sound", 0.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Skirt_Open", SOUNDCHANNEL::CHANNEL_26, 0.5f); }},
		});
	m_Model->Add_AnimNotify(ETOI(em3000State::DANMAK_START_3), {
		notify{L"Stop_Sound", 0.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_26); }},
		notify{L"Play_Sound", 0.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Skirt_Open", SOUNDCHANNEL::CHANNEL_26, 0.5f); }},
		});
	m_Model->Add_AnimNotify(ETOI(em3000State::DANMAK_END_1), {
		notify{L"Stop_Sound", 5.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_26); }},
		notify{L"Play_Sound", 5.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Skirt_Close_Round", SOUNDCHANNEL::CHANNEL_26, 0.5f); }},
		});
	m_Model->Add_AnimNotify(ETOI(em3000State::DANMAK_END_2), {
		notify{L"Stop_Sound", 5.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_26); }},
		notify{L"Play_Sound", 5.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Skirt_Close_Round", SOUNDCHANNEL::CHANNEL_26, 0.5f); }},
		});
	m_Model->Add_AnimNotify(ETOI(em3000State::DANMAK_END_3), {
		notify{L"Stop_Sound", 5.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_26); }},
		notify{L"Play_Sound", 5.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Skirt_Close", SOUNDCHANNEL::CHANNEL_26, 0.5f); }},
		});

	m_Model->Add_AnimNotify(ETOI(em3000State::FRONT_STOMP), {
		notify{L"Stop_Sound1", 0.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_26); }},
		notify{L"Play_Sound1", 0.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Stomp_Start", SOUNDCHANNEL::CHANNEL_26, 0.5f); }},

		notify{L"Stop_Sound2", 65.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_26); }},
		notify{L"Play_Sound2", 65.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Stomp_Impact", SOUNDCHANNEL::CHANNEL_26, 0.5f); }},
		notify{ L"Stomp_Impact", 65.f, [this, levIndex]()
		{
			Entity::DAMAGE_INFO dmgInfo{};
			dmgInfo.attacker = shared_from_this();
			dmgInfo.damage = 30.f;
			dmgInfo.groggyWeight = 20.f;
			dmgInfo.attackType = ATK_TYPE::HEAVY;
			dmgInfo.hitPosition = m_Owner.lock()->Get_Transform()->Get_Position();
			dmgInfo.hitRotation = Quaternion::Identity;
			dmgInfo.knockbackForce = 1.25f;

			MonsterShockWave::MONSTER_SHOCKWAVE_DESC desc{};
			desc.damageInfo = dmgInfo;
			desc.radius = 3.f;
			desc.position = dmgInfo.hitPosition + m_Owner.lock()->Get_Transform()->Get_Look() * desc.radius;
			GAME_INSTANCE->Instantiate<MonsterShockWave>(L"MonsterShockWave", levIndex, &desc);
		}},

		notify{L"Stop_Sound3", 100.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_26); }},
		notify{L"Play_Sound3", 100.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Stomp_End", SOUNDCHANNEL::CHANNEL_26, 0.5f); }}
		});

	m_Model->Add_AnimNotify(ETOI(em3000State::MIXER_START), {
		notify{L"Stop_Sound", 0.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_26); }},
		notify{L"Play_Sound", 0.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Mix_Start", SOUNDCHANNEL::CHANNEL_26, 0.5f); }},
		});
	m_Model->Add_AnimNotify(ETOI(em3000State::MIXING), {
		notify{L"Play_Sound", 0.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Mix_Loop", SOUNDCHANNEL::CHANNEL_26, 0.5f); }},
		notify{L"Mix_Attack", 0.f, 75.f, activeCenterAoe, deActiveCenterAoe},
		notify{L"Mix_Attack", 37.5f, activeCenterAoe},
		});
	m_Model->Add_AnimNotify(ETOI(em3000State::MIXER_END), {
		notify{L"Stop_Sound", 0.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_26); }},
		notify{L"Play_Sound", 0.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Mix_End", SOUNDCHANNEL::CHANNEL_26, 0.5f); }},
		notify{L"Mix_Attack_End", 0.f, [this, deActiveCenterAoe]() { deActiveCenterAoe(); }}
		});

	m_Model->Add_AnimNotify(ETOI(em3000State::DANMAK_LOOP_3), {
		notify{L"Howitzer1_Stop_Sound", 20.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_25); }},
		notify{L"Howitzer1_Play_Sound", 20.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Howitzer2", SOUNDCHANNEL::CHANNEL_25, 0.5f); }},
		notify{L"Howitzer1", 20.f, [this](){ Fire_Howitzer(8.f, 30.f); }},

		notify{L"Howitzer2_Stop_Sound", 70.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_25); }},
		notify{L"Howitzer2_Play_Sound", 70.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Howitzer3", SOUNDCHANNEL::CHANNEL_25, 0.5f); }},
		notify{L"Howitzer2", 70.f, [this]() { Fire_Howitzer(12.f, 30.f); }},
		
		notify{L"Howitzer3_Stop_Sound", 120.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_25); }},
		notify{L"Howitzer3_Play_Sound", 120.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Howitzer2", SOUNDCHANNEL::CHANNEL_25, 0.5f); }},
		notify{L"Howitzer3", 120.f, [this]() { Fire_Howitzer(8.f, 30.f); }},

		notify{L"Howitzer4_Stop_Sound", 170.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_25); }},
		notify{L"Howitzer4_Play_Sound", 170.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Howitzer1", SOUNDCHANNEL::CHANNEL_25, 0.5f); }},
		notify{L"Howitzer4", 170.f, [this]() { Fire_Howitzer(3.f, 30.f); }},

		notify{L"Howitzer5_Stop_Sound", 220.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_25); }},
		notify{L"Howitzer5_Play_Sound", 220.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Howitzer2", SOUNDCHANNEL::CHANNEL_25, 0.5f); }},
		notify{L"Howitzer5", 220.f, [this]() { Fire_Howitzer(8.f, 30.f); }},

		notify{L"Howitzer6_Stop_Sound", 270.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_25); }},
		notify{L"Howitzer6_Play_Sound", 270.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Howitzer1", SOUNDCHANNEL::CHANNEL_25, 0.5f); }},
		notify{L"Howitzer6", 270.f, [this]() { Fire_Howitzer(3.f, 30.f); }},

		notify{L"Howitzer7_Stop_Sound", 320.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_25); }},
		notify{L"Howitzer7_Play_Sound", 320.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Howitzer3", SOUNDCHANNEL::CHANNEL_25, 0.5f); }},
		notify{L"Howitzer7", 320.f, [this]() { Fire_Howitzer(12.f, 30.f); }}
	});

	m_Model->Add_AnimNotify(ETOI(em3000State::PHASE_CHANGE3), {
		notify{L"Transform1_Stop_Sound", 5.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_25); }},
		notify{L"Transform1_Sound", 5.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Skirt_Open_Short", SOUNDCHANNEL::CHANNEL_25, 0.5f); }},
		notify{L"Transform2_Stop_Sound", 45.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_25); }},
		notify{L"Transform2_Sound", 45.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Skirt_Open_Short", SOUNDCHANNEL::CHANNEL_25, 0.5f); }},
		notify{L"Transform3_Stop_Sound", 90.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_26); }},
		notify{L"Transform3_Sound", 90.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Nack_Appear", SOUNDCHANNEL::CHANNEL_26, 0.5f); }},
		notify{L"Transform4_Stop_Sound", 140.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_25); }},
		notify{L"Transform4_Sound", 140.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Event_Howling", SOUNDCHANNEL::CHANNEL_25, 0.5f); }},
		});

	m_Model->Add_AnimNotify(ETOI(em3000State::READY_CHASE), {
		notify{L"Back1_Stop_Sound", 20.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_25); }},
		notify{L"Back1_Sound", 20.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Em3000_Step1", SOUNDCHANNEL::CHANNEL_25, 0.5f); }},
		notify{L"Back2_Stop_Sound", 25.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_26); }},
		notify{L"Back2_Sound", 25.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Em3000_Step2", SOUNDCHANNEL::CHANNEL_26, 0.5f); }},
		notify{L"Back3_Stop_Sound", 30.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_25); }},
		notify{L"Back3_Sound", 30.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Em3000_Step1", SOUNDCHANNEL::CHANNEL_25, 0.5f); }},
		notify{L"Back4_Stop_Sound", 35.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_26); }},
		notify{L"Back4_Sound", 35.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Em3000_Step2", SOUNDCHANNEL::CHANNEL_26, 0.5f); }},

		notify{L"Chase1_Stop_Sound", 80.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_25); }},
		notify{L"Chase1_Sound", 80.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Em3000_Step1", SOUNDCHANNEL::CHANNEL_25, 0.5f); }},
		notify{L"Chase2_Stop_Sound", 85.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_26); }},
		notify{L"Chase2_Sound", 85.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Em3000_Step2", SOUNDCHANNEL::CHANNEL_26, 0.5f); }},
		});


	m_Model->Add_AnimNotify(ETOI(em3000State::CHASE), {
		notify{L"Chase1_Stop_Sound", 10.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_25); }},
		notify{L"Chase1_Sound", 10.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Em3000_Step1", SOUNDCHANNEL::CHANNEL_25, 0.5f); }},
		notify{L"Chase2_Stop_Sound", 25.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_26); }},
		notify{L"Chase2_Sound", 25.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Em3000_Step2", SOUNDCHANNEL::CHANNEL_26, 0.5f); }},
		notify{L"Chase3_Stop_Sound", 40.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_25); }},
		notify{L"Chase3_Sound", 40.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Em3000_Step1", SOUNDCHANNEL::CHANNEL_25, 0.5f); }},
		notify{L"Chase4_Stop_Sound", 55.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_26); }},
		notify{L"Chase4_Sound", 55.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Em3000_Step2", SOUNDCHANNEL::CHANNEL_26, 0.5f); }},
		notify{L"Chase5_Stop_Sound", 70.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_25); }},
		notify{L"Chase5_Sound", 70.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Em3000_Step1", SOUNDCHANNEL::CHANNEL_25, 0.5f); }},
		notify{L"Chase6_Stop_Sound", 85.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_26); }},
		notify{L"Chase6_Sound", 85.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Em3000_Step2", SOUNDCHANNEL::CHANNEL_26, 0.5f); }},
		});

	m_Model->Add_AnimNotify(ETOI(em3000State::JUMP_STOMP), {
		notify{L"Stomp1_Stop_Sound", 10.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_25); }},
		notify{L"Stomp1_Sound", 10.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"JumpStomp_Start", SOUNDCHANNEL::CHANNEL_25, 0.5f); }},
		notify{L"Stomp2_Stop_Sound", 80.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_25); }},
		notify{L"Stomp2_Sound", 80.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"JumpStomp_Fall", SOUNDCHANNEL::CHANNEL_25, 0.5f); }},
		notify{L"Stomp3_Stop_Sound", 90.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_25); }},
		notify{L"Stomp3_Sound", 90.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"JumpStomp_Impact", SOUNDCHANNEL::CHANNEL_25, 0.5f); }},
		notify{L"Stomp3_Sound", 90.f,[this, levIndex]() {
			Entity::DAMAGE_INFO dmgInfo{};
			dmgInfo.attacker = m_Owner.lock();
			dmgInfo.damage = 30.f;
			dmgInfo.groggyWeight = 20.f;
			dmgInfo.attackType = ATK_TYPE::HEAVY;
			dmgInfo.hitPosition = m_Owner.lock()->Get_Transform()->Get_Position();
			dmgInfo.hitRotation = Quaternion::Identity;
			dmgInfo.knockbackForce = 1.25f;

			MonsterShockWave::MONSTER_SHOCKWAVE_DESC desc{};
			desc.damageInfo = dmgInfo;
			desc.radius = 3.5f;
			desc.position = dmgInfo.hitPosition + m_Owner.lock()->Get_Transform()->Get_Look() * desc.radius;
			GAME_INSTANCE->Instantiate<MonsterShockWave>(L"MonsterShockWave", levIndex, &desc);
		}},
		notify{L"Stomp4_Stop_Sound", 120.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_25); }},
		notify{L"Stomp4_Sound", 120.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"JumpStomp_End", SOUNDCHANNEL::CHANNEL_25, 0.5f); }},
		});

	m_Model->Add_AnimNotify(ETOI(em3000State::SCREAMING), {
		notify{L"Screaming_Stop_Sound", 80.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_25); }},
		notify{L"Screaming_Sound", 80.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Event_Howling", SOUNDCHANNEL::CHANNEL_25, 0.5f); }},
		});

	m_Model->Add_AnimNotify(ETOI(em3000State::PHASE2_IDLE), {
		notify{L"Stop_Sound", 0.f, [this]() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_25); }},
		notify{L"Play_Sound", 0.f, [this]() { GAME_INSTANCE->PlaySoundFXOnce(L"Dizzy", SOUNDCHANNEL::CHANNEL_25, 0.5f); }},
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

void Em3000Body::Fire_Bullet(Bool useSin)
{
	m_IsFiring = true;
	m_IsCurveBullet = useSin;
	m_PortFireAccTime = 0.f;
	m_PermanentCount = 0;
	m_ModelMatrixAcc = Matrix::Identity;

}

void Em3000Body::Stop_Bullet()
{
	m_IsFiring = false;
	m_IsCurveBullet = false;
	m_PortFireAccTime = 0.f;
	m_PermanentCount = 0;
	m_ModelMatrixAcc = Matrix::Identity;
}

void Em3000Body::Fire_Howitzer(Float speed, Float gravity)
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
		howitzerDesc.speed = speed;
		howitzerDesc.maxDistance = 50.f;
		howitzerDesc.resourceTag = L"candy";
		howitzerDesc.targetLayer = L"Player";
		howitzerDesc.damage = 20.f;
		howitzerDesc.isPermanent = true;
		howitzerDesc.useCurvedFlight = true;
		howitzerDesc.gravityStrength = gravity;
		howitzerDesc.targetY = 21.f; // 지면 높이
		howitzerDesc.damageInfo = Projectile_DamageInfo();

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
	
	GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_19);
	GAME_INSTANCE->PlaySoundFXOnce(L"Default_Shot", SOUNDCHANNEL::CHANNEL_19, 0.5f);

	for (const auto& port : m_Ports)
	{
		Vector3 muzzleWorldPos = Vector3::Transform(port.muzzlePosition, worldMatrix);
		Vector3 muzzleWorldDir = Vector3::TransformNormal(port.direction, worldMatrix);

		muzzleWorldDir.Normalize();
		// [곡사포 Bullet 구조체 할당]
		Bullet::BULLET_DESC bulletDesc{};
		bulletDesc.initialPosition = muzzleWorldPos + Vector3{ 0.f, 1.75f, 0.f };
		bulletDesc.direction = muzzleWorldDir;
		bulletDesc.speed = 10.f;           // 전방으로 나아가는 힘
		bulletDesc.maxDistance = 18.5f;          // 수명 또는 사거리
		bulletDesc.resourceTag = L"candy";
		bulletDesc.targetLayer = L"Player";      // 맞출 대상
		bulletDesc.damage = 10.f;
		bulletDesc.isPermanent = isPermanent;
		bulletDesc.useUpperSin = m_IsCurveBullet;
		bulletDesc.damageInfo = Projectile_DamageInfo();
		bulletDesc.scale = Vector3{ 0.5f, 0.5f, 0.5f };

		auto bullet = GAME_INSTANCE->Instantiate<Bullet>(L"Bullet", GAME_INSTANCE->Get_TargetLevelIndex(), &bulletDesc);
	}
	++m_PermanentCount;
}

Entity::DAMAGE_INFO Em3000Body::Projectile_DamageInfo()
{
	Entity::DAMAGE_INFO dmgInfo{};
	dmgInfo.attackType = ATK_TYPE::HEAVY;
	dmgInfo.attacker = Get_Owner();
	dmgInfo.damage = 10.f;
	dmgInfo.groggyWeight = 1.f;
	dmgInfo.hitPosition = Get_Owner()->Get_Transform()->Get_Position();
	dmgInfo.hitRotation = Get_Owner()->Get_Transform()->Get_Quaternion();
	dmgInfo.knockbackForce = 1.f;
	
	return dmgInfo;
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

