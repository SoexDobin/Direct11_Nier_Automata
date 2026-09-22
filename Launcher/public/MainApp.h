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

#ifdef _DEBUG
	// 창 제목에 1초 평균 FPS/ms와 직전 프레임 draw·삼각형 수를 보인다.
	Float m_StatsTime{};
	uint32 m_StatsFrames{};
#endif

public:
	static Unique<MainApp> Create();
};

NS_END