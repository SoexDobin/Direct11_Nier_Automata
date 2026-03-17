#pragma once
#include "Level.h"
#include "Client_Define.h"


NS_BEGIN(Client)

class ClientApp;

class CLIENT_DLL Loader final : public Level
{
public:
	typedef struct tagLoaderDesc
	{
		LEVEL nextLevelID{};
		Shared<Level> ownerLevel{ nullptr };
	} LOADER_DESC;
public:
	Loader(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	~Loader() override;

public:
	Bool Is_Finished() const { return m_isFinished; }

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg = nullptr) override;
	void On_Destroy() override;
	
public:
	void Update_Level(Float timeDelta) override;
	HRESULT Render_Level() override;

public:
	HRESULT Loading();

private:
	HRESULT Loading_For_LogoLevel();
	HRESULT Loading_For_GamePlayLevel();
	HRESULT Loading_Global_Prototype();

private:
	HANDLE				m_Thread = { nullptr };
	LEVEL				m_NextLevelID = {LEVEL::LEVEL_END};
	Weak<Level>			m_OwnerLevel = {};
	CRITICAL_SECTION	m_CriticalSection = {};
	tChar				m_LoadingText[MAX_PATH] = {};

	Bool				m_isFinished = { false };

public:
	static Shared<Loader> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, LEVEL nextLevelID, const Shared<Level>& ownerLevel);

};

NS_END