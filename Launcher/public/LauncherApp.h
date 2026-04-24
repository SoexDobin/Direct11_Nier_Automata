#pragma once
#include <Engine_Define.h>

NS_BEGIN(Client)
class ClientApp;
NS_END

NS_BEGIN(Launcher)

class LauncherApp final 
{
public:
	explicit LauncherApp();
	~LauncherApp();
	
private:
	HRESULT Initialize();

public:
	void Update();
	void Render();

private:
	shared_ptr<Color> m_BackgroundColor{ nullptr };
	Unique<Client::ClientApp> m_ClientApp{nullptr};

public:
	static Unique<LauncherApp> Create();
};

NS_END
