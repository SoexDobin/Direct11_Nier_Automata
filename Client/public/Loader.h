#pragma once
#include "Level.h"
#include "Client_Define.h"

NS_BEGIN(Client)


// TODO : 한번더 나눠서 Level의 로더로 사용하는 편이 좋음
class Loader final : public Level, enable_shared_from_this<Loader>
{
public:
	Loader(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	~Loader() override;

public:
	Bool Is_Finished() const { return m_isFinished; }

public:
	HRESULT Initialize_Prototype() override = delete;
	HRESULT Initialize(void* arg = nullptr) override;
	void On_Destroy() override;
	
public:
	void Update_Level(Float timeDelta) override;
	HRESULT Render_Level() override = delete;

public:
	HRESULT Loading();

private:
	HRESULT Print_LoadingText();
	HRESULT Loading_For_LogoLevel();
	HRESULT Loading_For_GamePlayLevel();

private:
	HANDLE				m_Thread = { nullptr };
	LEVEL				m_NextLevelID = {LEVEL::LEVEL_END};
	CRITICAL_SECTION	m_CriticalSection = {};
	tChar				m_LoadingText[MAX_PATH] = {};
	BOOL				m_isFinished = { false };

public:
	static Shared<Loader> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);

};

NS_END