#pragma once
#include <Engine_Define.h>


NS_BEGIN(Client)
	class ClientApp;
NS_END

NS_BEGIN(Launcher)

class MainApp final 
{
public:
	explicit MainApp();
	~MainApp();
	
private:
	HRESULT Initialize();

public:
	void Update();
	void Render();

private:
	Unique<ClientApp> m_ClientApp{nullptr};

public:
	static Unique<MainApp> Create();
};

NS_END