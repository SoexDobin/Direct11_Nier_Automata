#include "pch.h"
#include "LevelGamePlay2.h"

#include <SpdLogger.h>

#include "ClientSettingManager.h"
#include "NavigationManager.h"
#include "WorldTriggerManager.h"

LevelGamePlay2::LevelGamePlay2(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Level{ device, context }
{
}

HRESULT LevelGamePlay2::Initialize(void* arg)
{
	WorldTriggerManager::GetInstance()->Reset_NavigationSettingFlag();
	NavigationManager::GetInstance()->Reset_NavigationSettingFlag();

	if (FAILED(ClientSettingManager::GetInstance()->Load_LevelData(LEVEL::GAMEPLAY2)))
	{
		LOG_CRITICAL(L"Failed To Load GamePlay2 Level");
		return E_FAIL;
	}

	return Level::Initialize(arg);
}

void LevelGamePlay2::On_Destroy()
{
	Level::On_Destroy();
}

void LevelGamePlay2::Update_Level(Float timeDelta)
{
	NavigationManager::GetInstance()->Setting_NavigationSector(LEVEL::GAMEPLAY2);
	WorldTriggerManager::GetInstance()->Setting_Trigger(LEVEL::GAMEPLAY2);

	Level::Update_Level(timeDelta);
}

HRESULT LevelGamePlay2::Render_Level()
{
	return Level::Render_Level();
}

Shared<LevelGamePlay2> LevelGamePlay2::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto levGamePlay = make_shared<LevelGamePlay2>(device, context);

	if (FAILED(levGamePlay->Initialize(nullptr)))
	{
		LOG_ERROR(L"Failed to Create GamePlay2 Level");
		return nullptr;
	}

	return levGamePlay;
}
