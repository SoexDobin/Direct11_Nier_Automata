#pragma once
#include "Level.h"

NS_BEGIN(Client)

class Loader;

class CLIENT_DLL LevelGamePlay2 final : public Level
{
public:
	LevelGamePlay2(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	~LevelGamePlay2() override = default;

private:
	HRESULT Initialize_Prototype() override { return S_OK; }
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;

public:
	void Update_Level(Float timeDelta) override;
	HRESULT Render_Level() override;

private:

public:
	static Shared<LevelGamePlay2> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);

};

NS_END

