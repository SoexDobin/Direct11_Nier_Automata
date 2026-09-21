#include "WorldCollider.h"

#include "Game.h"
#include "GameObject.h"
#include "SpdLogger.h"
#include "Transform.h"

WorldCollider::WorldCollider() : Component{} {}
WorldCollider::WorldCollider(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Component{ device, context } {}
WorldCollider::WorldCollider(const WorldCollider& rhs)
	: Component{ rhs }, m_SourceModelTag{ rhs.m_SourceModelTag } {}

HRESULT WorldCollider::Initialize_Prototype()
{
	return Component::Initialize_Prototype();
}

HRESULT WorldCollider::Initialize(void* arg)
{
	if (FAILED(Component::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize WorldCollider");
		return E_FAIL;
	}

	if (auto* desc = static_cast<WORLD_COLLIDER_DESC*>(arg))
		m_SourceModelTag = desc->sourceModelTag;

	return S_OK;
}

HRESULT WorldCollider::Post_Load()
{
	if (m_SourceModelTag.empty())
	{
		LOG_WARN(L"[WorldCollider] No source model tag; nothing registered");
		return S_OK;
	}

	const Shared<GameObject> owner = Get_Owner();
	if (nullptr == owner)
		return E_FAIL;

	m_ActorHandle = GAME_INSTANCE->Add_StaticCollision(
		m_SourceModelTag, owner->Get_Transform()->Get_WorldMatrix());

	if (0 == m_ActorHandle)
	{
		LOG_ERROR(L"[WorldCollider] Failed to register static collision for {}", m_SourceModelTag);
		return E_FAIL;
	}

	return S_OK;
}

void WorldCollider::On_Destroy()
{
	if (0 != m_ActorHandle)
	{
		GAME_INSTANCE->Remove_StaticCollision(m_ActorHandle);
		m_ActorHandle = 0;
	}

	Component::On_Destroy();
}

Shared<WorldCollider> WorldCollider::CreatePrototype()
{
	auto collider = make_shared<WorldCollider>(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context());

	if (FAILED(collider->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Initialize Prototype : WorldCollider");
		return nullptr;
	}

	return collider;
}

Shared<WorldCollider> WorldCollider::Create(const ComPtr<ID3D11Device>& device,
	const ComPtr<ID3D11DeviceContext>& context)
{
	auto collider = make_shared<WorldCollider>(device, context);

	if (FAILED(collider->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : WorldCollider");
		return nullptr;
	}

	return collider;
}

Shared<Component> WorldCollider::Clone(void* arg)
{
	auto instance = make_shared<WorldCollider>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Cloned : WorldCollider");
		return nullptr;
	}

	return instance;
}
