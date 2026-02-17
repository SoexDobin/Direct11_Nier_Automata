#include "pch.h"
#include "LevelLoading.h"

LevelLoading::LevelLoading(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Level { device, context }, m_Loader { nullptr }, m_NextLevel { LEVEL::}
{
}

HRESULT LevelLoading::Initialize_Prototype()
{
	return Level::Initialize_Prototype();
}

HRESULT LevelLoading::Initialize(const Shared<void>& arg)
{
	return Level::Initialize(arg);
}

void LevelLoading::On_Destroy()
{
	Level::On_Destroy();
}

void LevelLoading::Update_Level(Float timeDelta)
{
	Level::Update_Level(timeDelta);
}

HRESULT LevelLoading::Render_Level()
{
	return Level::Render_Level();
}

HRESULT LevelLoading::Ready_BackGround()
{
}

HRESULT LevelLoading::Ready_UI()
{
}

Shared<LevelLoading> LevelLoading::Create(const ComPtr<ID3D11Device>& device,
	const ComPtr<ID3D11DeviceContext>& context, LEVEL nextLevelID)
{
}
