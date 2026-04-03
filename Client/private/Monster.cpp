#include "pch.h"
#include "Monster.h"

#include <Game.h>
#include <Transform.h>

#include "SpdLogger.h"

Monster::Monster() : Entity{} {}
Monster::Monster(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Entity{device, context} {}
Monster::Monster(const Monster& rhs)
	: Entity{rhs} {}

HRESULT Monster::Initialize_Prototype()
{
	m_LayerMask.Set_Layer(L"MonsterPhysical");
	m_TagMask.Set_Tag({ L"Monster" });

	return Entity::Initialize_Prototype();
}

HRESULT Monster::Initialize(void* arg)
{
	if (FAILED(Entity::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize Monster {}", m_ObjectName);
		return E_FAIL;
	}

	return S_OK;
}

void Monster::On_Destroy()
{
	GameObject::On_Destroy();
}

void Monster::On_Enable()
{
	GameObject::On_Enable();
}

void Monster::On_Disable()
{
	GameObject::On_Disable();
}

void Monster::Priority_Update(Float timeDelta)
{

}

void Monster::Update(Float timeDelta)
{
	
}

void Monster::Late_Update(Float timeDelta)
{
}

void Monster::Fixed_Update(Float fixedDelta)
{
	
}

HRESULT Monster::Render()
{
	return S_OK;
}

void Monster::Submit_RenderGroup()
{
}

