#include "pch.h"
#include "MainApp.h"

MainApp::MainApp() {}

HRESULT MainApp::Initialize(void *arg) 
{
	Object::Initialize(arg);

	m_pGameInstance;
}

void MainApp::Update()
{
	
}

HRESULT MainApp::Render()
{
	
	return S_OK;
}

Unique<MainApp> MainApp::Create() {
	auto mainApp = make_unique<MainApp>();

	if (FAILED(mainApp->Initialize())) 
	{
		MSG_BOX("Failed To Create MainApp");
		return nullptr;
	}

	return mainApp;
}

void MainApp::OnDestroy() {}
