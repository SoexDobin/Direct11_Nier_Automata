#pragma once
#include "Object.h"

NS_BEGIN(Engine)
	class Game;
NS_END

NS_BEGIN(Client)

class MainApp final : public Object
{
public:
	MainApp();
	~MainApp() override = default;

public:
	HRESULT Initialize(void* arg = nullptr) override;
	void Update();
	HRESULT Render();

private:
	void OnDestroy() override;

private:
	Weak<Game>					m_pGameInstance = {};
	ComPtr<ID3D11Device>		m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>	m_pContext = { nullptr };

private:
	//HRESULT Ready_StartLevel(LEVEL eStartLevelID);

public:
	static Unique<MainApp> Create();
	
};

NS_END