#include "pch.h"
#include "Pl0000MonsterChecker.h"

#include <Game.h>

#include "Pl0000.h"
#include <SpdLogger.h>
#include "SphereCollider.h"

Pl0000MonsterChecker::Pl0000MonsterChecker() : PartObject{} {}
Pl0000MonsterChecker::Pl0000MonsterChecker(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: PartObject{device, context} {}
Pl0000MonsterChecker::Pl0000MonsterChecker(const Pl0000MonsterChecker& rhs)
	: PartObject{rhs} {}

Shared<GameObject> Pl0000MonsterChecker::Get_ClosestTarget()
{
	if (m_Pl0000.expired()) return nullptr;

	auto player = m_Pl0000.lock();
	Vector3 pos = player->Get_Transform()->Get_Position();
	Vector3 look = player->Get_Transform()->Get_Look();
	look.y = 0.f;
	look.Normalize();

	Shared<GameObject> bestTarget{ nullptr };
	Float minDistance = FLT_MAX;

	if (m_CheckedMonsters.empty()) return nullptr;

	for (auto it = m_CheckedMonsters.begin(); it != m_CheckedMonsters.end(); )
	{
		if (it->expired())
		{
			it = m_CheckedMonsters.erase(it);
			continue;
		}

		auto targetMonster = it->lock();
		if (targetMonster->Is_Destroy() || !targetMonster->Is_Active())
		{
			it = m_CheckedMonsters.erase(it);
			continue;
		}

		Vector3 targetPos = it->lock()->Get_Transform()->Get_Position();
		Vector3 toTarget = targetPos - pos;
		toTarget.y = 0.f;

		Float distance = toTarget.Length();
		if (distance > 0.001f)
			toTarget.Normalize();

		Float dot = look.Dot(toTarget);

		if (dot > 0.f && distance < minDistance)
		{
			minDistance = distance;
			bestTarget = targetMonster;
		}

		++it;
	}

	return bestTarget;
}

HRESULT Pl0000MonsterChecker::Initialize_Prototype()
{
	m_LayerMask.Set_UseCustomMask(true);
	m_LayerMask.ClearMask();
	m_LayerMask.Set_Mask(L"MonsterPhysical");

	return PartObject::Initialize_Prototype();
}

HRESULT Pl0000MonsterChecker::Initialize(void* arg)
{
	if (FAILED(PartObject::Initialize(arg)))
	{
		LOG_ERROR(L"Failed To Initialize Pl0000MonsterChecker");
		return S_OK;
	}

	SphereCollider::SPHERE_COLLIDER_DESC colDesc{};
	colDesc.radius = m_CheckDistance;
	colDesc.offset = Vector3::UnitY;
	m_EvadeChecker = Add_Component<SphereCollider>(ETOI(LEVEL::STATIC), &colDesc);
	if (m_EvadeChecker == nullptr)
		return E_FAIL;

	if (m_Owner.expired())
		return E_FAIL;
	m_Pl0000 = static_pointer_cast<Pl0000>(m_Owner.lock());

	m_TargetLayerIndex = ETOI(GAME_INSTANCE->Get_LayerRegister()->Get_LayerByName(L"MonsterPhysical"));

	return S_OK;
}

HRESULT Pl0000MonsterChecker::Begin()
{


	return S_OK;
}

void Pl0000MonsterChecker::On_Destroy()
{
	m_CheckedMonsters.clear();

	PartObject::On_Destroy();
}

void Pl0000MonsterChecker::Update(Float timeDelta)
{
	m_Transform->Update_WorldMatrix();
	Update_CombineWorldMatrix(*m_Transform->Get_WorldMatrixPtr());
	m_EvadeChecker->Update(m_CombinedWorldMatrix);
}

void Pl0000MonsterChecker::OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	if (m_Pl0000.expired()) return;

	auto target = targetCollider->Get_Owner();

	if (target->Get_LayerMask().Get_Layer() != m_TargetLayerIndex) return;

	for (auto& monster : m_CheckedMonsters)
	{
		if (monster.expired()) continue;
		if (monster.lock() == target) return;
	}

	m_CheckedMonsters.push_back(target);
}

void Pl0000MonsterChecker::OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
	

}

void Pl0000MonsterChecker::OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	if (m_Pl0000.expired()) return;

	auto target = targetCollider->Get_Owner();

	if (target->Get_LayerMask().Get_LayerName() != L"MonsterPhysical") return;

	m_CheckedMonsters.remove_if([&target](const Weak<GameObject>& monster)
		{
			return monster.expired() || monster.lock() == target;
		});
}

Shared<Pl0000MonsterChecker> Pl0000MonsterChecker::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<Pl0000MonsterChecker>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Pl0000MonsterChecker");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> Pl0000MonsterChecker::Clone(void* arg)
{
	auto instance = make_shared<Pl0000MonsterChecker>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : Pl0000MonsterChecker");
		return nullptr;
	}

	return instance;
}



