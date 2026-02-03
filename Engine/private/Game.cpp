#include "Game.h"
#include "GraphicDevice.h"
#include "TimeManager.h"
#include "LevelManager.h"
#include "PrototypeManager.h"
#include "ObjectManager.h"


IMPLEMENT_SINGLETON(Game)

Game::Game() {}

Game::~Game() {}

HRESULT Game::Initialize_Engine(const ENGINE_DESC& engineDesc,
                                _Out_ ComPtr<ID3D11Device>& device,
                                _Out_ ComPtr<ID3D11DeviceContext>& context)
{
	m_GraphicDevice = GraphicDevice::Create(
		engineDesc.hWnd, 
		engineDesc.winMode,
		engineDesc.viewportWidth, 
		engineDesc.viewportHeight,
		device, 
		context
	);
	
	if (nullptr == (m_TimeManager = TimeManager::Create()))
		return E_FAIL;

	if (nullptr == (m_PrototypeManager = PrototypeManager::Create(engineDesc.levCount)))
		return E_FAIL;

	if (nullptr == (m_ObjectManager = ObjectManager::Create(engineDesc.levCount)))
		return E_FAIL;


	return S_OK;
}

void Game::Update_Engine()
{
	Float delta = m_TimeManager->Update_Timers();

	
}

HRESULT Game::Draw()
{

	return S_OK;
}

void Game::Clear_Resource(uint32 levIndex) 
{

}
