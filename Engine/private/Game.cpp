#include "Game.h"
#include "GraphicDevice.h"
#include "TimeManager.h"

IMPLEMENT_SINGLETON(Game)

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
	m_TimeManager = TimeManager::Create();

	return S_OK;
}

void Game::Update_Engine() const
{
}

HRESULT Game::Draw() const {

	return S_OK;
}

void Game::Clear_Resource() {

}
