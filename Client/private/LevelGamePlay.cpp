#include "pch.h"
#include "LevelGamePlay.h"

#include <SpdLogger.h>

#include "ClientSettingManager.h"
#include "NavigationManager.h"

LevelGamePlay::LevelGamePlay(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Level{ device, context }
{
}

HRESULT LevelGamePlay::Initialize(void* arg)
{
	NavigationManager::GetInstance()->Reset_NavigationSettingFlag();

	if (FAILED(ClientSettingManager::GetInstance()->Load_LevelData(LEVEL::GAMEPLAY)))
	{
		LOG_CRITICAL(L"Failed To Load GamePlay Level");
		return E_FAIL;
	}

	return Level::Initialize(arg);
}

void LevelGamePlay::On_Destroy()
{
	Level::On_Destroy();
}

void LevelGamePlay::Update_Level(Float timeDelta)
{
	NavigationManager::GetInstance()->Setting_NavigationSector(LEVEL::GAMEPLAY);

	Level::Update_Level(timeDelta);
}

HRESULT LevelGamePlay::Render_Level()
{
	return Level::Render_Level();
}

Shared<LevelGamePlay> LevelGamePlay::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto levGamePlay = make_shared<LevelGamePlay>(device, context);

	if (FAILED(levGamePlay->Initialize(nullptr)))
	{
		LOG_ERROR(L"Failed to Create GamePlay Level");
		return nullptr;
	}

	return levGamePlay;
}
