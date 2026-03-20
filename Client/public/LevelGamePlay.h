#pragma once
#include "Level.h"

NS_BEGIN(Client)

class Loader;

class CLIENT_DLL LevelGamePlay final : public Level
{
public:
	LevelGamePlay(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	~LevelGamePlay() override = default;

private:
	HRESULT Initialize_Prototype() override { return S_OK; }
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;

public:
	void Update_Level(Float timeDelta) override;
	HRESULT Render_Level() override;

private:
	LEVEL			m_NextLevel = {};
	Shared<Loader>	m_Loader = { nullptr };

public:
	static Shared<LevelGamePlay> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, LEVEL nextLevelID);

};

NS_END

