#pragma once
#include "Level.h"

NS_BEGIN(Client)

class Loader;

class LevelLoading final : public Level
{
public:
	LevelLoading(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	~LevelLoading() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(const Shared<void>& arg) override;
	void On_Destroy() override;
	void Update_Level(Float timeDelta) override;
	HRESULT Render_Level() override;

private:
	HRESULT Ready_BackGround();
	HRESULT Ready_UI();

private:
	Shared<Loader> m_Loader = { nullptr };

public:
	static Shared<LevelLoading> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, LEVEL nextLevelID);
	
};

NS_END