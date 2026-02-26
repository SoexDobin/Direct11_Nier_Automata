#pragma once
#include "Client_Define.h"
#include "Engine_Define.h"


NS_BEGIN(Engine)
class Game;
NS_END

NS_BEGIN(Client)

class ClientApp final 
{
public:
	ClientApp();
	~ClientApp();

public:
	HRESULT Initialize(const ENGINE_DESC& desc);
	void Update() const;
	HRESULT Render() const;

private:
	HRESULT Ready_StartLevel(LEVEL startLevel);

public:
	static Unique<ClientApp> Create(const ENGINE_DESC& desc);
};

NS_END