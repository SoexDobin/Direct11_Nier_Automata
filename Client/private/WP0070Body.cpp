#include "pch.h"
#include "WP0070Body.h"

#include <SpdLogger.h>

#include "Game.h"
#include "Model.h"
#include "OBBCollider.h"
#include "Entity.h"
#include "Navigation.h"
#include "Monster.h"
#include "Pl0000.h"
#include "Pl0000Shockwave.h"

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

	if (FAILED(Ready_AnimationNotify()))
	{
		LOG_ERROR(L"Failed To Ready Animation Notify : WP0070Body");
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
	m_Pl0000 = static_pointer_cast<Pl0000>(m_Owner.lock());

	m_MonsterLayer = ETOI(GAME_INSTANCE->Get_LayerRegister()->Get_LayerByName(L"Monster"));

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
	Float actualTimeDelta = timeDelta;
	if (auto entity = dynamic_pointer_cast<Entity>(m_Owner.lock())) {
		if (entity->Get_LagDuration() > 0.f) actualTimeDelta *= 0.05f;
	}

	m_Model->Update_ModelAnimation(actualTimeDelta);
}

void WP0070Body::Late_Update(Float timeDelta)
{
	uint32 animIndex = m_Model->Get_AnimationIndex();
	Bool isFinished = m_Model->Is_AnimationFinished();

	if (!m_DetachTransform
		&& animIndex == ETOI(WP0070_STATE::LIGHT_GROUND7)
		&& isFinished)
	{
		m_DetachTransform = true;
	}

	if (m_DetachTransform)
	{
		if (!m_DetachTransform)
		{
			m_DetachTransform = true;
		}

		if (!m_IsHitTheGround)
		{
			Float speed = 50.f;
		
			Vector3 lookDir = m_Pl0000.lock()->Get_Body()->Get_Transform()->Get_Look();
			lookDir.y = 0.f;
			lookDir.Normalize();

			Quaternion quat = Quaternion::CreateFromAxisAngle(Vector3::UnitX, 45.f);

			lookDir = Vector3::Transform(lookDir, quat);

			m_CombinedWorldMatrix *= Matrix::CreateTranslation(lookDir * speed * timeDelta);
			Vector3 worldNextPos = m_CombinedWorldMatrix.Translation();

			if (worldNextPos.y <= m_Owner.lock()->Get_Transform()->Get_Position().y - 1.5f)
			{
				m_IsHitTheGround = true;

				GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_10);
				GAME_INSTANCE->PlaySoundFXOnce(L"Wp0070_Impact7", SOUNDCHANNEL::CHANNEL_10, 0.5f);
				Impact_Shockwave(Vector3::Zero);

				return;
			}
		}
		Matrix boneMatrix = m_Model->Get_BoneMatrix(m_WeaponBoneIndex);
		m_AttackCollider->Update(boneMatrix * m_CombinedWorldMatrix);
		return;
	}

	Update_CombineWorldMatrix(*m_Transform->Get_WorldMatrixPtr());
	Matrix boneMatrix = m_Model->Get_BoneMatrix(m_WeaponBoneIndex);
	m_AttackCollider->Update(boneMatrix * m_CombinedWorldMatrix);
}

void WP0070Body::Fixed_Update(Float fixedDelta)
{

}

HRESULT WP0070Body::Render()
{
	if (m_IsActive == false) return S_OK;

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

void WP0070Body::Submit_RenderGroup()
{
	if (m_IsActive == false) return;
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

void WP0070Body::OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}

void WP0070Body::OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	if (!m_AttackCollider->Is_Active()) return;

	auto target = targetCollider->Get_Owner();
	if (target->Get_GameObjectType() != GAMEOBJECTTYPE::PART) return;
	if (target->Get_LayerMask().Get_Layer() != m_MonsterLayer) return;

	uint32 targetID = target->Get_InstanceID();
	if (m_HitEntities.contains(targetID) == false)
	{
		m_HitEntities.insert(targetID);

		Vector3 boneScale{};
		Quaternion boneQuat{};
		Vector3 boneTranslation{};
		m_Model->Get_BoneMatrix(m_WeaponBoneIndex).Decompose(boneScale, boneQuat, boneTranslation);

		Entity::DAMAGE_INFO dmgInfo{};
		dmgInfo.attacker = m_Owner.lock();
		dmgInfo.damage = 100.f;
		dmgInfo.groggyWeight = 100;
		dmgInfo.attackType = ATK_TYPE::LIGHT;
		dmgInfo.hitPosition = targetCollider->ClosestPoint(ownCollider->Get_Pivot());
		dmgInfo.hitRotation = m_AttackCollider->Get_CurrentOrientation();
		dmgInfo.knockbackForce = 1.5f;

		auto mon = static_pointer_cast<PartObject>(target)->Get_Owner();
		static_pointer_cast<Entity>(mon)->TakeDamage(dmgInfo);

		auto monster = static_pointer_cast<Monster>(static_pointer_cast<PartObject>(target)->Get_Owner());
		monster->TakeDamage(dmgInfo);
		monster->OnAttackHit(shared_from_this());
		monster->Add_HitLag(0.05f);

		auto attackerEntity = static_pointer_cast<Entity>(dmgInfo.attacker.lock());
		attackerEntity->OnAttackHit(monster);
		attackerEntity->Add_HitLag(0.05f);
	}
}

void WP0070Body::OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}

void WP0070Body::Set_Sheathing()
{
	m_DetachTransform = false;
	m_IsHitTheGround = false;

	if (m_IsSheathing) return;

	m_AttackCollider->Set_Active(false);
	m_IsSheathing = true;
	m_IsActive = false;
	Set_Animation(0, 0.f, false);
}

void WP0070Body::DrawWP0070()
{
	if (m_IsSheathing == false) return;

	m_Transform->Set_WorldMatrix(Matrix::Identity);
	m_IsSheathing = false;
}

void WP0070Body::Set_Animation(uint32 animIndex, Float blendDuration, Bool isLoop)
{
	m_DetachTransform = false;
	m_IsHitTheGround = false;

	m_Transform->Set_WorldMatrix(Matrix::Identity);
	Pl0000Parts::Set_Animation(animIndex, blendDuration, isLoop);
	m_Model->Update_ModelAnimation(0.001f);
}

void WP0070Body::Active_LightWeapon()
{
	m_AttackCollider->Set_Active(true);
	m_HitEntities.clear();
}

void WP0070Body::DeActive_LightWeapon()
{
	m_AttackCollider->Set_Active(false);
	m_HitEntities.clear();
}

void WP0070Body::Impact_Shockwave(const Vector3& offset)
{
	Matrix boneMatrix = m_Model->Get_BoneMatrix(m_WeaponBoneIndex);
	Matrix worldMatrix = boneMatrix * m_CombinedWorldMatrix;

	Vector3 truePos = Vector3::Transform(offset, worldMatrix);

	Entity::DAMAGE_INFO dmgInfo{};
	dmgInfo.attacker = m_Owner.lock();
	dmgInfo.damage = 125.f;
	dmgInfo.groggyWeight = 125.f;
	dmgInfo.attackType = ATK_TYPE::LIGHT;
	dmgInfo.hitPosition = truePos;
	dmgInfo.hitRotation = Quaternion::Identity;
	dmgInfo.knockbackForce = 2.f;

	uint32 levIndex = GAME_INSTANCE->Get_TargetLevelIndex();
	Pl0000Shockwave::PLAYER_SHOCKWAVE_DESC desc{};
	desc.damageInfo = dmgInfo;
	desc.position = truePos;
	desc.radius = 2.f;
	GAME_INSTANCE->Instantiate<Pl0000Shockwave>(L"Pl0000Shockwave", levIndex, &desc);
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
	colDesc.extents = Vector3{ 0.075f, 0.1f, 1.f }; 
	colDesc.rotation = Vector3::Zero;
	colDesc.offset = Vector3{ 0.f, 0.f, -0.55f };
	m_AttackCollider = Add_Component<OBBCollider>(ETOI(LEVEL::STATIC), &colDesc);
	if (nullptr == m_AttackCollider)
		return E_FAIL;
	
	return S_OK;
}

HRESULT WP0070Body::Ready_AnimationNotify()
{
	using Notify = AnimationTracker::ANIMATION_NOTIFY;

	auto active = [this]() { Active_LightWeapon(); };
	auto deActive = [this]() { DeActive_LightWeapon(); };

	m_Model->Add_AnimNotify(ETOI(WP0070_STATE::LIGHT_GROUND_RUN), {
	Notify{ L"Sound_Stop", 5, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_10); } },
	Notify{ L"Sound_Swing", 5, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0070_Swing1", SOUNDCHANNEL::CHANNEL_10, 0.5f); } },
		Notify{L"Swing1", 10, 20, active, deActive }, 
		});

	m_Model->Add_AnimNotify(ETOI(WP0070_STATE::LIGHT_GROUND1), {
	Notify{ L"Sound_Stop", 5, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_10); } },
	Notify{ L"Sound_Swing", 5, [](){ GAME_INSTANCE->PlaySoundFXOnce(L"Wp0070_Swing1", SOUNDCHANNEL::CHANNEL_10, 0.5f);} },
		Notify{L"Swing1", 10, 20, active, deActive },
		});

	m_Model->Add_AnimNotify(ETOI(WP0070_STATE::LIGHT_GROUND2), {
	Notify{ L"Sound_Stop", 5, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_10); } },
	Notify{ L"Sound_Swing", 5, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0070_Swing2", SOUNDCHANNEL::CHANNEL_10, 0.5f); } },
		Notify{L"Swing2", 10, 20, active, deActive },
		});

	m_Model->Add_AnimNotify(ETOI(WP0070_STATE::LIGHT_GROUND3), {
	Notify{ L"Sound_Stop", 5, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_9); } },
	Notify{ L"Sound_Stop", 10, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_10); } },
	Notify{ L"Sound_Catch", 5, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0070_Catch3", SOUNDCHANNEL::CHANNEL_9, 0.3f); } },
	Notify{ L"Sound_Throw", 10, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0070_Throw3", SOUNDCHANNEL::CHANNEL_10, 0.5f); } },
		Notify{L"Swing3", 10, 100, active, deActive },
		Notify{L"Swing3", 30, active,  },
		Notify{L"Swing3", 40, active,  },
		Notify{L"Swing3", 45, active,  },
		Notify{L"Swing3", 50, active },
		});

	m_Model->Add_AnimNotify(ETOI(WP0070_STATE::LIGHT_GROUND4), {
	Notify{ L"Sound_Stop", 20, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_9); } },
	Notify{ L"Sound_Stop", 25, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_10); } },
	Notify{ L"Sound_Catch", 20, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0070_Catch4", SOUNDCHANNEL::CHANNEL_9, 0.3f); } },
	Notify{ L"Sound_Throw", 25, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0070_Throw4", SOUNDCHANNEL::CHANNEL_10, 0.5f); } },
		Notify{L"Swing4", 25, 100, active, deActive },
		Notify{L"Swing4", 30, active,  },
		Notify{L"Swing4", 40, active,  },
		Notify{L"Swing4", 45, active,  },
		Notify{L"Swing4", 50, active },
		});

	m_Model->Add_AnimNotify(ETOI(WP0070_STATE::LIGHT_GROUND5), {
	Notify{ L"Sound_Stop", 20, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_9); } },
	Notify{ L"Sound_Stop", 25, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_10); } },
	Notify{ L"Sound_Return", 20, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0070_Return5", SOUNDCHANNEL::CHANNEL_9, 0.3f); } },
	Notify{ L"Sound_Throw", 25, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0070_Throw5", SOUNDCHANNEL::CHANNEL_10, 0.5f); } },
		Notify{L"Swing5", 25, 100, active, deActive },
		Notify{L"Swing5", 30, active,  },
		Notify{L"Swing5", 40, active,  },
		Notify{L"Swing5", 45, active,  },
		Notify{L"Swing5", 50, active },
		});

	m_Model->Add_AnimNotify(ETOI(WP0070_STATE::LIGHT_GROUND6), {
	Notify{ L"Sound_Stop", 20, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_9); } },
	Notify{ L"Sound_Stop", 20, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_10); } },
	Notify{ L"Sound_Catch", 20, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0070_Catch6", SOUNDCHANNEL::CHANNEL_9, 0.3f); } },
	Notify{ L"Sound_Swing", 20, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0070_Swing6", SOUNDCHANNEL::CHANNEL_10, 0.5f); } },
		Notify{L"Swing6", 20, 50, active, deActive },
		});

	m_Model->Add_AnimNotify(ETOI(WP0070_STATE::LIGHT_GROUND7), {
	Notify{ L"Sound_Stop", 5, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_10); } },
	Notify{ L"Sound_Throw", 5, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0070_Throw7", SOUNDCHANNEL::CHANNEL_10, 0.5f); } }
		});

	m_Model->Add_AnimNotify(ETOI(WP0070_STATE::LIGHT_GROUND_HOLD), {
	Notify{ L"Sound_Stop", 10, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_10); } },
	Notify{ L"Sound_Hold", 10, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0070_Hold1", SOUNDCHANNEL::CHANNEL_10, 0.5f); } },
	Notify{ L"Sound_Stop", 45, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_10); } },
	Notify{ L"Sound_Hold", 45, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0070_Hold2", SOUNDCHANNEL::CHANNEL_10, 0.5f); } },
		Notify{L"Hold", 20, 60, active, deActive },
		Notify{L"Hold", 33, active },
		Notify{L"Hold", 46, active },
		Notify{L"Hold", 59, active },
		});

	m_Model->Add_AnimNotify(ETOI(WP0070_STATE::LIGHT_COMBO), {
	Notify{ L"Sound_Stop", 10, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_10); } },
	Notify{ L"Sound_Swing", 10, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0070_Swing1", SOUNDCHANNEL::CHANNEL_10, 0.5f); } },
	Notify{ L"Sound_Stop", 25, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_10); } },
	Notify{ L"Sound_Swing", 25, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0070_Swing2", SOUNDCHANNEL::CHANNEL_10, 0.5f); } },
	Notify{ L"Sound_Stop", 60, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_10); } },
	Notify{ L"Sound_Swing", 60, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0070_Hold2", SOUNDCHANNEL::CHANNEL_10, 0.5f); } },
		Notify{L"Combo", 10, 70, active, deActive },
		Notify{L"Combo", 25, active },
		Notify{L"Combo", 60, active },
		});

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


