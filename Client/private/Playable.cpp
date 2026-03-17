#include "pch.h"
#include "Playable.h"

Playable::Playable() : GameObject{} {}

Playable::Playable(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: GameObject{device, context} {}

Playable::Playable(const Playable& rhs)
	: GameObject{rhs} {}

HRESULT Playable::Initialize_Prototype()
{
	return GameObject::Initialize_Prototype();
}

HRESULT Playable::Initialize(void* arg)
{
	return GameObject::Initialize(arg);
}

void Playable::On_Destroy()
{
	GameObject::On_Destroy();
}

void Playable::On_Enable()
{
	GameObject::On_Enable();
}

void Playable::On_Disable()
{
	GameObject::On_Disable();
}

void Playable::Priority_Update(Float timeDelta)
{
	GameObject::Priority_Update(timeDelta);
}

void Playable::Update(Float timeDelta)
{
	GameObject::Update(timeDelta);
}

void Playable::Late_Update(Float timeDelta)
{
	GameObject::Late_Update(timeDelta);
}

void Playable::Fixed_Update(Float fixedDelta)
{
	GameObject::Fixed_Update(fixedDelta);
}

HRESULT Playable::Render()
{
	return GameObject::Render();
}

void Playable::Submit_RenderGroup()
{
	GameObject::Submit_RenderGroup();
}

