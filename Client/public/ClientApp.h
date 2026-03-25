#pragma once
#include "Client_Define.h"
#include "Engine_Define.h"

NS_BEGIN(Engine)
class Game;
NS_END

NS_BEGIN(Client)

class CLIENT_DLL ClientApp final 
{
public:
	ClientApp();
	~ClientApp();

public:
	uint32 Get_LevelCount() const { return ETOI(LEVEL::LEVEL_END); }

public:
	HRESULT Initialize(const ENGINE_DESC& desc);

private:
	HRESULT Ready_StartLevel(LEVEL startLevel);
	HRESULT Ready_InitialObject(); 

public:
	static Unique<ClientApp> Create(const ENGINE_DESC& desc);
};

NS_END