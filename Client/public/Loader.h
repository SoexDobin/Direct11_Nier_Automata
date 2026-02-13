#pragma once
#include "Object.h"
#include "Client_Define.h"

NS_BEGIN(Client)

class Loader final : public Object
{
public:
	Loader(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	~Loader() override;

public:
	Bool Is_Finished() const { return m_isFinished; }

public:
	PROTOTYPE Get_Prototype() const override { return PROTOTYPE::OBJECT; };
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(const Shared<void>& arg) override;
	void On_Destroy() override;
	
private:
	HRESULT Loading();
	HRESULT Loading_For_LogoLevel();
	HRESULT Loading_For_GamePlayLevel();

private:


	HANDLE				m_Thread = { nullptr };
	LEVEL				m_NextLevelID = { LEVEL::LEVEL_END };
	CRITICAL_SECTION	m_CriticalSection = {};
	tChar				m_LoadingText[MAX_PATH] = {};
	BOOL				m_isFinished = { false };

public:
	static Shared<Loader> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);

};

NS_END