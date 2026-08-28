#include "pch.h"
#include "WP0220Body.h"

#include <SpdLogger.h>

#include "Game.h"
#include "Model.h"
#include "Monster.h"
#include "OBBCollider.h"
#include "Pl0000Shockwave.h"
#include "Entity.h"

WP0220Body::WP0220Body() : Pl0000Parts{} {}
WP0220Body::WP0220Body(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Pl0000Parts{ device, context } {}
WP0220Body::WP0220Body(const WP0220Body& rhs)
	: Pl0000Parts{ rhs } {}

HRESULT WP0220Body::Initialize_Prototype()
{
	m_LayerMask.Set_Layer(L"PlayerWeapon");
	m_TagMask.Set_Tag({L"PlayerWeapon" });

	return Pl0000Parts::Initialize_Prototype();
}

HRESULT WP0220Body::Initialize(void* arg)
{
	if (FAILED(Pl0000Parts::Initialize(arg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed To Ready_Components : P10000Body");
		return E_FAIL;
	}

	if (FAILED(Ready_AnimationNotify()))
	{
		LOG_ERROR(L"Failed To Ready Animation Notify : WP0220Body");
		return E_FAIL;
	}

	m_RootBoneIndex = m_Model->Get_BoneIndexByName("wp0220");
	m_WeaponBoneIndex = m_Model->Get_BoneIndexByName("bone0");
	if (m_RootBoneIndex == -1)
	{
		LOG_ERROR(L"Failed to Find WP0220 Root Bone");
		return E_FAIL;
	}

	m_Model->Set_LocalRootNode(m_RootBoneIndex);

	return S_OK;
}

void WP0220Body::On_Destroy()
{
	PartObject::On_Destroy();
}

void WP0220Body::Priority_Update(Float timeDelta)
{

}

void WP0220Body::Update(Float timeDelta)
{
	Float actualTimeDelta = timeDelta;
	if (auto entity = dynamic_pointer_cast<Entity>(Get_Owner())) {
		if (entity->Get_LagDuration() > 0.f) actualTimeDelta *= 0.05f;
	}
	m_Model->Update_ModelAnimation(actualTimeDelta);

	if (m_IsSheathing == false)
	{
		TRANSFORM_FRAME rootVelocity = m_Model->Get_RootTransformVelocity(m_WeaponBoneIndex);

	}
}

void WP0220Body::Late_Update(Float timeDelta)
{
	Update_CombineWorldMatrix(*m_Transform->Get_WorldMatrixPtr());
	
	Matrix boneMatrix = m_Model->Get_BoneMatrix(m_WeaponBoneIndex);
	m_AttackCollider->Update(boneMatrix * m_CombinedWorldMatrix);
}

void WP0220Body::Fixed_Update(Float fixedDelta)
{

}

HRESULT WP0220Body::Render()
{
	if (m_IsActive == false) return S_OK;

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

void WP0220Body::Submit_RenderGroup()
{
	if (m_IsActive == false) return;
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

void WP0220Body::OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{

}

void WP0220Body::OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	if (!m_AttackCollider->Is_Active()) return;


	auto target = targetCollider->Get_Owner();
	if (target->Get_GameObjectType() != GAMEOBJECTTYPE::PART) return;
	if (target->Get_LayerMask().Get_LayerName() != L"Monster") return;

	uint32 targetID = target->Get_InstanceID();
	if (m_HitEntities.contains(targetID) == false)
	{
		m_HitEntities.insert(targetID);

		Entity::DAMAGE_INFO dmgInfo{};
		dmgInfo.attacker = Get_Owner();
		dmgInfo.damage = 175.f;
		dmgInfo.groggyWeight = 175.f;
		dmgInfo.attackType = ATK_TYPE::HEAVY;
		dmgInfo.hitPosition = targetCollider->ClosestPoint(ownCollider->Get_Pivot());
		dmgInfo.hitRotation = m_AttackCollider->Get_CurrentOrientation();
		dmgInfo.knockbackForce = 2.f;

		auto monster = static_pointer_cast<Monster>(static_pointer_cast<PartObject>(target)->Get_Owner());
		monster->TakeDamage(dmgInfo);
		monster->OnAttackHit(shared_from_this());
		monster->Add_HitLag(0.2f);

		auto attackerEntity = static_pointer_cast<Entity>(dmgInfo.attacker.lock());
		attackerEntity->OnAttackHit(monster);
		attackerEntity->Add_HitLag(0.2f);

	}
}

void WP0220Body::OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{

}

void WP0220Body::Active_LightWeapon()
{
	m_AttackCollider->Set_Active(true);
	m_HitEntities.clear();
}

void WP0220Body::DeActive_LightWeapon()
{
	m_AttackCollider->Set_Active(false);
	m_HitEntities.clear();
}

void WP0220Body::Impact_Shockwave(const Vector3& offset)
{
	Matrix boneMatrix = m_Model->Get_BoneMatrix(m_WeaponBoneIndex);
	Matrix worldMatrix = boneMatrix * m_CombinedWorldMatrix; 

	Vector3 truePos = Vector3::Transform(offset, worldMatrix);

	Entity::DAMAGE_INFO dmgInfo{};
	dmgInfo.attacker = Get_Owner();
	dmgInfo.damage = 225.f;
	dmgInfo.groggyWeight = 225.f;
	dmgInfo.attackType = ATK_TYPE::HEAVY;
	dmgInfo.hitPosition = truePos;
	dmgInfo.hitRotation = Quaternion::Identity;
	dmgInfo.knockbackForce = 3.5f;

	Pl0000Shockwave::PLAYER_SHOCKWAVE_DESC desc{};
	desc.damageInfo = dmgInfo;
	desc.position = truePos;
	desc.radius = 3.f;
	GAME_INSTANCE->Instantiate<Pl0000Shockwave>(L"Pl0000Shockwave", ETOI(LEVEL::GAMEPLAY), &desc);
}

void WP0220Body::Set_Sheathing()
{
	if (m_IsSheathing) return;
	m_AttackCollider->Set_Active(false);
	m_IsSheathing = true;
}

void WP0220Body::DrawWP0220()
{
	if (m_IsSheathing == false) return;
	m_AttackCollider->Set_Active(true);
	m_IsSheathing = false;
}

void WP0220Body::Set_Animation(uint32 animIndex, Float blendDuration, Bool isLoop)
{
	m_Transform->Set_WorldMatrix(Matrix::Identity);
	Pl0000Parts::Set_Animation(animIndex, blendDuration, isLoop);
	m_Model->Update_ModelAnimation(0.0001f);
}

HRESULT WP0220Body::Bind_ShaderResources()
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

HRESULT WP0220Body::Ready_Components()
{
	Shader::SHADER_DESC shaderDesc{ VTXANIMMESH::Tag,  VTXANIMMESH::Elements, VTXANIMMESH::numElements };
	m_Shader = Add_Component<Shader>(ETOI(LEVEL::STATIC), &shaderDesc);
	if (nullptr == m_Shader)
		return E_FAIL;

	Model::MODEL_DESC modelDesc{ L"wp0220" };
	m_Model = Add_Component<Model>(ETOI(LEVEL::STATIC), &modelDesc);
	if (nullptr == m_Model)
		return E_FAIL;

	OBBCollider::OBB_COLLIDER_DESC colDesc{};
	colDesc.extents = Vector3{ 0.25f, 0.05f, 0.8f };
	colDesc.rotation = Vector3::Zero;
	colDesc.offset = Vector3{ -0.05f, 0.f, -1.f }; 
	m_AttackCollider = Add_Component<OBBCollider>(ETOI(LEVEL::STATIC), &colDesc);
	if (nullptr == m_AttackCollider)
		return E_FAIL;

	return S_OK;
}

HRESULT WP0220Body::Ready_AnimationNotify()
{
	using Notify = AnimationTracker::ANIMATION_NOTIFY;

	auto active = [this]() { Active_LightWeapon(); };
	auto deActive = [this]() { DeActive_LightWeapon(); };
	auto impactShockwave = [this]() { Impact_Shockwave(Vector3{0.f, 0.f, -2.f }); };
	auto impactCombo = [this]() { Impact_Shockwave(Vector3{0.f, 0.f, -2.f}); };

	m_Model->Add_AnimNotify(ETOI(WP0220_STATE::HEAVY_GROUND1), {
	Notify{ L"Wp0220_Stop1", 30, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_12); } },
	Notify{ L"Wp0220_Swing1_1", 30, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0220_Swing1_1", SOUNDCHANNEL::CHANNEL_12, 0.5f); } },
	Notify{ L"Wp0220_Stop2", 60, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_12); } },
	Notify{ L"Wp0220_Swing1_2", 60, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0220_Swing1_2", SOUNDCHANNEL::CHANNEL_12, 0.5f); } },
		Notify{L"Swing1", 30, 100, active, deActive },
		Notify{L"Swing1", 60, active,  },
		});

	m_Model->Add_AnimNotify(ETOI(WP0220_STATE::HEAVY_GROUND2), {
	Notify{ L"Wp0220_Stop1", 65, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_12); } },
	Notify{ L"Wp0220_Swing2_1", 65, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0220_Swing2_1", SOUNDCHANNEL::CHANNEL_12, 0.5f); } },
		Notify{L"Swing2", 65, 100, active, deActive },
		Notify{L"Swing2", 80, active  },
		});

	m_Model->Add_AnimNotify(ETOI(WP0220_STATE::HEAVY_GROUND3), {
	Notify{ L"Wp0220_Stop1", 45, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_12); } },
	Notify{ L"Wp0220_Swing3_1", 45, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0220_Swing3_1", SOUNDCHANNEL::CHANNEL_12, 0.5f); } },
	Notify{ L"Wp0220_Stop2", 65, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_12); } },
	Notify{ L"Wp0220_Swing3_2", 65, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0220_Swing3_2", SOUNDCHANNEL::CHANNEL_12, 0.5f); } },
	Notify{ L"Wp0220_Stop3", 108, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_12); } },
	Notify{ L"Wp0220_Swing3_3", 108, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0220_Swing3_3", SOUNDCHANNEL::CHANNEL_12, 0.5f); } },
		Notify{L"Swing3", 50, 70, active, deActive },
		Notify{L"Swing3", 60, active,  },
		Notify{L"Impact", 111, impactShockwave}
		// 충격파 111 프레임일때 bone위치에 Instant
		});

	m_Model->Add_AnimNotify(ETOI(WP0220_STATE::HEAVY_COMBO), {
	Notify{ L"Wp0220_Stop1", 40, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_12); } },
	Notify{ L"Wp0220_Swing3_2", 40, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0220_Swing3_2", SOUNDCHANNEL::CHANNEL_12, 0.5f); } },
	Notify{ L"Wp0220_Stop1", 75, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_12); } },
	Notify{ L"Wp0220_Hammer", 75, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0220_Hammer", SOUNDCHANNEL::CHANNEL_12, 0.5f); } },
		Notify{L"Swing3", 40, 85, active, deActive },
		Notify{L"Swing3", 80, active },
		Notify{L"Impact", 80, impactCombo }
		});

	m_Model->Add_AnimNotify(ETOI(WP0220_STATE::HEAVY_GROUND_HOLD_UNFULL), {
	Notify{ L"Wp0220_Stop1", 40, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_12); } },
	Notify{ L"Wp0220_Charge_Impact", 40, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0220_Charge_Impact", SOUNDCHANNEL::CHANNEL_12, 0.5f); } },
		Notify{L"Impact", 40, impactShockwave}
		// 충격파
		});

	m_Model->Add_AnimNotify(ETOI(WP0220_STATE::HEAVY_GROUND_HOLD_FULL), {
	Notify{ L"Wp0220_Stop1", 0, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_12); } },
	Notify{ L"Wp0220_Charge_Swing1", 0, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0220_Charge_Swing1", SOUNDCHANNEL::CHANNEL_12, 0.5f); } },
	Notify{ L"Wp0220_Stop2", 20, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_12); } },
	Notify{ L"Wp0220_Charge_Swing2", 20, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0220_Charge_Swing2", SOUNDCHANNEL::CHANNEL_12, 0.5f); } },
	Notify{ L"Wp0220_Stop3", 30, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_12); } },
	Notify{ L"Wp0220_Charge_Swing3", 30, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0220_Charge_Swing3", SOUNDCHANNEL::CHANNEL_12, 0.5f); } },
	Notify{ L"Wp0220_Stop4", 40, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_12); } },
	Notify{ L"Wp0220_Charge_Swing4", 40, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0220_Charge_Swing4", SOUNDCHANNEL::CHANNEL_12, 0.5f); } },
		Notify{L"Swing1", 0, 50, active, deActive },
		Notify{L"Swing2", 20, active },
		Notify{L"Swing2", 30, active },
		Notify{L"Swing2", 40, active },
		});

	m_Model->Add_AnimNotify(ETOI(WP0220_STATE::HEAVY_AIR_DOWN_END), {
		Notify{L"Wp0220_Stop1", 0, []() { GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_12); }},
		Notify{L"Wp0220_Air_Down", 0, []() { GAME_INSTANCE->PlaySoundFXOnce(L"Wp0220_Hammer", SOUNDCHANNEL::CHANNEL_12, 0.5f); } },
		Notify{L"Impact", 10, impactShockwave}
		});

	return S_OK;
}

Shared<WP0220Body> WP0220Body::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<WP0220Body>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : WP0220Body");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> WP0220Body::Clone(void* arg)
{
	auto instance = make_shared<WP0220Body>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : WP0220Body");
		return nullptr;
	}

	return instance;
}


