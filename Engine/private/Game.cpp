#include "Game.h"
#include "GraphicDevice.h"
#include "LevelManager.h"
#include "ObjectManager.h"
#include "PrototypeManager.h"
#include "Renderer.h"
#include "TimeManager.h"

#include "Level.h"
#include "SpdLogger.h"
#include "Timer.h"
#include "SpdLogger.h"

NS_BEGIN(Engine)

IMPLEMENT_SINGLETON(Game);

Game::~Game()
{

	LOG_SHUTDOWN(); /* Debug Helper SpdLogger */

}

HRESULT Game::Initialize_Engine(const ENGINE_DESC &engineDesc,
                                _Out_ ComPtr<ID3D11Device> &device,
                                _Out_ ComPtr<ID3D11DeviceContext> &context) 
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

	if (nullptr == (m_LevelManager = LevelManager::Create()))
        return E_FAIL;

    if (nullptr ==(m_PrototypeManager = PrototypeManager::Create(engineDesc.levCount)))
		return E_FAIL;

	if (nullptr == (m_ObjectManager = ObjectManager::Create()))
		return E_FAIL;

	if (nullptr == (m_Renderer = Renderer::Create(device, context)))
		return E_FAIL;

	LOG_INIT(); /* Debug Helper SpdLogger */ 

	return S_OK;
}

void Game::Update_Engine() 
{
	const Float delta = m_TimeManager->Update_Timers();

	m_ObjectManager->PriorityUpdate(delta);

	m_ObjectManager->Update(delta);

	m_ObjectManager->LateUpdate(delta);

	while (m_TimeManager->Is_FixedUpdate()) 
	{
		Float fixedDelta = m_TimeManager->Get_MainTimer()->GetFixedDeltaTime();
		m_ObjectManager->FixedUpdate(fixedDelta);
		m_TimeManager->Get_MainTimer()->ConsumeFixedDeltaTime();
		m_TimeManager->Has_FixedUpdate();
	}
}

HRESULT Game::Draw() 
{
	if (m_TimeManager->Is_FixedUpdate())
		m_Renderer->Draw();

	return S_OK;
}

void Game::Clear_Resource(uint32 levIndex) 
{
	if (FAILED(m_PrototypeManager->Clear_Prototypes(levIndex))) 
	{
		LOG_CRITICAL(L"Failed To Clear Level{} Prototypes", levIndex);
	}

	if (FAILED(m_ObjectManager->Clear_GameObjects())) 
	{
		LOG_CRITICAL(L"Failed To Clear GameObjects");
	}
}

HRESULT Game::Clear_BackBufferView(const Shared<Float4> &clearColor) const 
{
	if (m_TimeManager->Is_FixedUpdate())
		return S_OK;
		
	if (FAILED(m_GraphicDevice->Clear_BackBufferView(clearColor)))
		return E_FAIL;
	if (FAILED(m_GraphicDevice->Clear_DepthStencilView()))
		return E_FAIL;

	return S_OK;
}

HRESULT Game::Present() const
{
	if (m_TimeManager->Is_FixedUpdate())
		return m_GraphicDevice->Present();
	
	return S_OK;
}

HRESULT Game::Add_Timer(const wstring &timerTag) const 
{
	if (FAILED(m_TimeManager->Add_Timer(timerTag))) {
		MSG_BOX("Failed To Add Timer");
		return E_FAIL;
	}

	return S_OK;
}

Float Game::Compute_TimeDelta(const wstring &timerTag) const 
{
	return m_TimeManager->Get_Timer(timerTag)->GetDeltaTime();
}

HRESULT Game::Change_Level(uint32 levIndex, Unique<Level> newLevel) 
{
	if (FAILED(m_LevelManager->Change_Level(levIndex, std::move(newLevel)))) 
	{
		MSG_BOX("Change To New Level Got a Trouble");
		return E_FAIL;
	}

  // TODO : Cache 객체들 관리
  // Static Level을 통한 객체 처리를 어찌 해볼까
  // 아니면 오브젝트 매니저에서 등록해서 따로 뭐시기 할까
  return S_OK;
}

HRESULT Game::Add_Prototype(uint32 levIndex, const Shared<Object> &prototype) const 
{
	if (FAILED(m_PrototypeManager->Add_Prototype(levIndex, prototype))) 
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Game::Add_GameObject(const Shared<GameObject>& gameObject) const
{
	if (FAILED(m_ObjectManager->Add_GameObject(gameObject)))
	{
		LOG_ERROR(L"Failed To Add GameObject");
		return E_FAIL;
	}

	return S_OK;
}

void Game::Add_RenderGroup(RENDERGROUP group, const Shared<GameObject> &gameObject) const 
{
	m_Renderer->Add_RenderGroup(group, gameObject);
}

NS_END


#include "Game.inl"