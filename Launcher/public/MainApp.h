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
	// F7로 켜는 우상단 프레임 표시: 1초 평균 FPS/ms와 직전 프레임 draw·삼각형 수.
	Float m_StatsTime{};
	uint32 m_StatsFrames{};
	wchar_t m_StatsText[176]{};
	Bool m_ShowFrameStats{ false };
	Bool m_PrevF7{ false };
	Bool m_PrevF8{ false };
	Bool m_PrevF9{ false };
#endif

public:
	static Unique<MainApp> Create();
};

NS_END