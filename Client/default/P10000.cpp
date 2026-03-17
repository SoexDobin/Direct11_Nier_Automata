#include "pch.h"
#include "P10000.h"

#include <SpdLogger.h>

P10000::P10000()
{
}

P10000::P10000(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
}

P10000::P10000(const P10000& rhs)
{
}

HRESULT P10000::Initialize_Prototype()
{
	return Playable::Initialize_Prototype();
}

HRESULT P10000::Initialize(void* arg)
{
	return Playable::Initialize(arg);
}

void P10000::On_Destroy()
{
	Playable::On_Destroy();
}

void P10000::Priority_Update(Float timeDelta)
{
	Playable::Priority_Update(timeDelta);
}

void P10000::Update(Float timeDelta)
{
	Playable::Update(timeDelta);
}

void P10000::Late_Update(Float timeDelta)
{
	Playable::Late_Update(timeDelta);
}

void P10000::Fixed_Update(Float fixedDelta)
{
	Playable::Fixed_Update(fixedDelta);
}

HRESULT P10000::Render()
{
	return Playable::Render();
}

void P10000::Submit_RenderGroup()
{
	Playable::Submit_RenderGroup();
}

Shared<P10000> P10000::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	Shared<P10000> defaultMonster = make_shared<P10000>(device, context);

	if (FAILED(defaultMonster->Initialize_Prototype()))
	{
		LOG_ERROR(L"Failed to Created : 2B");
		MSG_BOX("Failed to Created : 2B");
	}
	return defaultMonster;
}

Shared<GameObject> P10000::Clone(void* arg)
{
	Shared<P10000> defaultMonster = make_shared<P10000>(*this);

	if (FAILED(defaultMonster->Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Cloned : 2B");
		MSG_BOX("Failed to Cloned : 2B");
	}
	return defaultMonster;
}
