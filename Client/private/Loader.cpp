#include "Loader.h"


Loader::Loader(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: 
{
}

Loader::~Loader()
{
	On_Destroy();
}

HRESULT Loader::Initialize_Prototype()
{
	return Object::Initialize_Prototype();
}

HRESULT Loader::Initialize(const Shared<void>& arg)
{
	return Object::Initialize(arg);
}

void Loader::On_Destroy()
{
	WaitForSingleObject(m_Thread, INFINITE);
	CloseHandle(m_Thread);
	DeleteCriticalSection(&m_CriticalSection);

	Object::On_Destroy();
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
