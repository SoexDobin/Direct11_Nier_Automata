#include "pch.h"
#include "LoadingBackground.h"

#include "SpdLogger.h"

LoadingBackground::LoadingBackground() : UIObject() {}

LoadingBackground::LoadingBackground(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: UIObject { device, context }
{
}

LoadingBackground::LoadingBackground(const Shared<LoadingBackground>& rhs)
	: UIObject{ rhs }
{
}

HRESULT LoadingBackground::Initialize_Prototype()
{
	return UIObject::Initialize_Prototype();
}

HRESULT LoadingBackground::Initialize(void* arg)
{
	return UIObject::Initialize(arg);
}

void LoadingBackground::On_Destroy()
{
	UIObject::On_Destroy();
}

void LoadingBackground::Priority_Update(Float timeDelta)
{
	UIObject::Priority_Update(timeDelta);
}

void LoadingBackground::Update(Float timeDelta)
{
	UIObject::Update(timeDelta);
}

void LoadingBackground::Late_Update(Float timeDelta)
{
	UIObject::Late_Update(timeDelta);
}

void LoadingBackground::Fixed_Update(Float fixedDelta)
{
	UIObject::Fixed_Update(fixedDelta);
}

HRESULT LoadingBackground::Render()
{
	return UIObject::Render();
}

Shared<LoadingBackground> LoadingBackground::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto bg = make_shared<LoadingBackground>();

	if (FAILED(bg->Initialize_Prototype()))
	{
		LOG_ERROR(L"Failed To Create LoadingBackground");
		return nullptr;
	}

	return bg;
}

Shared<GameObject> LoadingBackground::Clone(void* arg)
{
	auto self = ;
	return make_shared<LoadingBackground>(enable_shared_from_this<LoadingBackground>::shared_from_this());
}
