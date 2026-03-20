#include "pch.h"
#include "LevelGamePlay.h"

#include <SpdLogger.h>

LevelGamePlay::LevelGamePlay(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Level{ device, context }
{
}

HRESULT LevelGamePlay::Initialize(void* arg)
{
	return Level::Initialize(arg);
}

void LevelGamePlay::On_Destroy()
{
	Level::On_Destroy();
}

void LevelGamePlay::Update_Level(Float timeDelta)
{
	Level::Update_Level(timeDelta);
}

HRESULT LevelGamePlay::Render_Level()
{
	return Level::Render_Level();
}

Shared<LevelGamePlay> LevelGamePlay::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, LEVEL nextLevelID)
{
	auto levGamePlay = make_shared<LevelGamePlay>(device, context);

	if (FAILED(levGamePlay->Initialize(&nextLevelID)))
	{
		LOG_ERROR(L"Failed to Create GamePlay Level");
		return nullptr;
	}

	return levGamePlay;
}
