#include "GameObject.h"

GameObject::GameObject(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& context)
	: m_Device(pDevice), m_Context(context)
{

}

GameObject::GameObject(const Shared<GameObject>& prototype)
	: GameObject(prototype->m_Device, prototype->m_Context)
{

}

HRESULT GameObject::Initialize_Prototype() { return S_OK; }

HRESULT GameObject::Initialize(Shared<void> arg)
{


	return S_OK;
}

void GameObject::Priority_Update(Float timeDelta) {}

void GameObject::Update(Float timeDelta) {}

void GameObject::Late_Update(Float timeDelta) {}

HRESULT GameObject::Render() { return S_OK; }
