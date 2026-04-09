#include "pch.h"
#include "Em0010Movement.h"
#include "Em0010.h"
#include <SpdLogger.h>

Em0010Movement::Em0010Movement() : Movement{} {}
Em0010Movement::Em0010Movement(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Movement{device, context} {}
Em0010Movement::Em0010Movement(const Em0010Movement& rhs)
	: Movement{rhs} {}

HRESULT Em0010Movement::Initialize_Prototype()
{
	return Movement::Initialize_Prototype();
}

HRESULT Em0010Movement::Initialize(void* arg)
{
	if (FAILED(Movement::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Init {}", m_ObjectName);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Em0010Movement::Begin()
{
	if (!m_Owner.expired())
	{
		m_OwnerContainer = static_pointer_cast<Em0010>(m_Owner.lock());
		if (m_OwnerContainer.expired())
		{
			LOG_ERROR(L"Failed To Find Em0010 Container");
			return E_FAIL;
		}
	}

	return S_OK;
}

void Em0010Movement::Update_Movement(Float timeDelta)
{
	
}

Shared<Em0010Movement> Em0010Movement::Create(const ComPtr<ID3D11Device>& device,
                                              const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<Em0010Movement>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Em0010Movement");
		return nullptr;
	}

	return prototype;
}

Shared<Component> Em0010Movement::Clone(void* arg)
{
	auto instance = make_shared<Em0010Movement>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : Em0010Movement");
		return nullptr;
	}

	return instance;
}


