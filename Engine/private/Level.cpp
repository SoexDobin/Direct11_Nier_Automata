#include "Level.h"

Level::Level(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: m_Device(device), m_Context(context)
{
}

HRESULT Level::Initialize(Shared<void> arg)
{
	return Object::Initialize(arg);
}

void Level::On_Destroy()
{
	
}

void Level::On_Disable()
{
	
}

void Level::On_Enable()
{
	
}

void Level::Update_Level(Float timeDelta)
{
}

HRESULT Level::Render_Level()
{
	return S_OK;
}
