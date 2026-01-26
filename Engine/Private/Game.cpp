#include "Game.h"
#include "GraphicDevice.h"

IMPLEMENT_SINGLETON(Game)

HRESULT Game::Initialize_Engine(const ENGINE_DESC &engineDesc,
								_Out_ ComPtr<ID3D11Device> &device,
								_Out_ ComPtr<ID3D11DeviceContext> &context)
{

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
