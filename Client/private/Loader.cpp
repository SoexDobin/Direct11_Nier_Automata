#include "pch.h"
#include "Loader.h"

Loader::Loader(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Level { device, context }
{
}


uint32 APIENTRY ThreadMain(void* arg)
{
	Shared<Loader>* pLoaderPtr = static_cast<Shared<Loader>*>(arg);
	std::unique_ptr<Shared<Loader>> loaderPtrGuard(pLoaderPtr);
	Shared<Loader> loader = *pLoaderPtr;

	if (!loader)
		return 1;
	if (FAILED(loader->Loading()))
		return 1;

	return 0; 
}
HRESULT Loader::Initialize(void* arg)
{
	m_NextLevelID = static_cast<LEVEL>(reinterpret_cast<uint32>(arg));
	InitializeCriticalSection(&m_CriticalSection);

	// shared_ptr 복사본을 힙에 할당
	Shared<Loader>* pLoaderCopy = new Shared<Loader>(shared_from_this());

	m_Thread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr,
	                                                   0,
	                                                   ThreadMain,
	                                                   pLoaderCopy,
	                                                   0,
	                                                   nullptr));

	if (m_Thread == nullptr)
	{
		delete pLoaderCopy; // 실패 시 메모리 정리
		return E_FAIL;
	}
	return Level::Initialize(arg);
}

void Loader::On_Destroy()
{
	WaitForSingleObject(m_Thread, INFINITE);
	CloseHandle(m_Thread);
	DeleteCriticalSection(&m_CriticalSection);

	Level::On_Destroy();
}

void Loader::Update_Level(Float timeDelta)
{
	Level::Update_Level(timeDelta);
}

HRESULT Loader::Loading()
{

}

HRESULT Loader::Print_LoadingText()
{
}

HRESULT Loader::Loading_For_LogoLevel()
{
	m_isFinished = false;


	// TODO :  

	m_isFinished = true;
	return S_OK;
}

HRESULT Loader::Loading_For_GamePlayLevel()
{
	m_isFinished = false;

	// TODO :  

	m_isFinished = true;
	return S_OK;
}

Shared<Loader> Loader::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	Shared<Loader> loader = make_shared<Loader>(device, context);

	if (FAILED(loader->Initialize(nullptr)))
	{
		MSG_BOX("Failed to Created : Loader");
	}

	return loader;
}
