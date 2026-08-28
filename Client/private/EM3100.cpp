#include "pch.h"
#include "Em3100.h"

#include <Game.h>

#include "Bullet.h"
#include "Em3100Body.h"
#include "HpBarWorldUI.h"
#include "SphereCollider.h"
#include "SpdLogger.h"
#include "Random_Helper.h"


Em3100::Em3100() : Monster{} {}
Em3100::Em3100(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Monster{device, context} {}
Em3100::Em3100(const Em3100& rhs)
	: Monster{rhs} {}

HRESULT Em3100::Initialize_Prototype()
{
	if (FAILED(Monster::Initialize_Prototype()))
	{
		LOG_ERROR(L"Failed to Create Prototype Monster");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Em3100::Initialize(void* arg)
{
	if (FAILED(Monster::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize em3100");
		return E_FAIL;
	}

	if (FAILED(Ready_PartObjects()))
	{
		LOG_ERROR(L"Failed to Ready PartObjects em3100 ");
		return E_FAIL;
	}

	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed to Ready Components em3100 ");
		return E_FAIL;
	}

	GAME_INSTANCE->Add_Instance_Event(ETOI(LEVEL::GAMEPLAY), L"Add_HpBar", [this]()
		{
			HpBarWorldUI::HP_BAR_WORLD_UI_DESC UI_hpDesc{};
			UI_hpDesc.target = static_pointer_cast<Entity>(shared_from_this());
			UI_hpDesc.worldOffset = Vector3{ 0.f, 1.f, 0.f };
			UI_hpDesc.anchor = UI_ANCHOR::TOP_LEFT;
			UI_hpDesc.sizeX = 200.f;
			UI_hpDesc.sizeY = 10.f;
			UI_hpDesc.x = 0.f;
			UI_hpDesc.y = 0.f;
			m_HpBarUI = GAME_INSTANCE->Instantiate<HpBarWorldUI>(L"HpBarWorldUI", ETOI(LEVEL::GAMEPLAY), &UI_hpDesc);
		});


	return S_OK;
}

void Em3100::Priority_Update(Float timeDelta)
{

}

void Em3100::Update(Float timeDelta)
{
	m_Transform->Update_WorldMatrix();
	m_InteractionZone->Update(m_Transform->Get_WorldMatrix());
}

void Em3100::Late_Update(Float timeDelta)
{
	
}

void Em3100::Fixed_Update(Float fixedDelta)
{
	
}

HRESULT Em3100::Render()
{
	return S_OK;
}

void Em3100::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

void Em3100::TakeDamage(const DAMAGE_INFO& dmgInfo)
{
	Play_HitSFX(dmgInfo);
	DisplaySparkEffect(dmgInfo.attackType, dmgInfo.hitPosition, dmgInfo.hitRotation);

	Entity::TakeDamage(dmgInfo);
}

void Em3100::OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}

void Em3100::OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{

}

void Em3100::OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}


HRESULT Em3100::Ready_PartObjects()
{
	Em3100Body::EM3100BODY_DESC em3100BodyDesc{};

	if (FAILED(Add_Child(ETOI(LEVEL::GAMEPLAY), L"Em3100Body", L"Em3100Body", &em3100BodyDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT Em3100::Ready_Components()
{
	SphereCollider::SPHERE_COLLIDER_DESC sphereDesc{};
	sphereDesc.radius = 3.f;
	sphereDesc.offset = Vector3::UnitY;
	m_InteractionZone = Add_Component<SphereCollider>(ETOI(LEVEL::STATIC), &sphereDesc);
	if (nullptr == m_InteractionZone)
		return E_FAIL;

	return S_OK;
}

Shared<Em3100> Em3100::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<Em3100>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : EM3100");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> Em3100::Clone(void* arg)
{
	auto instance = make_shared<Em3100>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : EM3100");
		return nullptr;
	}

	return instance;
}


