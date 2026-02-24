#include "pch.h"
#include "LevelLoading.h"

#include <SpdLogger.h>

LevelLoading::LevelLoading(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Level { device, context }, m_Loader { nullptr }, m_NextLevel { LEVEL::LEVEL_END }
{
}

HRESULT LevelLoading::Initialize_Prototype()
{
	return Level::Initialize_Prototype();
}

HRESULT LevelLoading::Initialize(void* arg)
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


	return S_OK;
}

HRESULT LevelLoading::Ready_UI()
{


	return S_OK;
}

Unique<LevelLoading> LevelLoading::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, LEVEL nextLevelID)
{
	auto loadingLevel = make_unique<LevelLoading>(device, context);

	if (FAILED(loadingLevel->Initialize(reinterpret_cast<void*>(nextLevelID)))) {
		LOG_ERROR(L"Failed To Create LoadingBackground");
		return nullptr;
	}

	return loadingLevel;
}
