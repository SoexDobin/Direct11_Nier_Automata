#pragma once
#include "Level.h"

NS_BEGIN(Client)

class LoadingFade;

class CLIENT_DLL LevelTitle final : public Level
{
public:
	LevelTitle(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	~LevelTitle() override = default;

private:
	HRESULT Initialize_Prototype() override { return S_OK; }
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;

public:
	void Update_Level(Float timeDelta) override;
	HRESULT Render_Level() override;

private:
	void Ready_LoadingUI();

private:
	Shared<LoadingFade>	m_FadeIn{ nullptr };
	Shared<LoadingFade>	m_FadeOut{ nullptr };

public:
	static Shared<LevelTitle> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, LEVEL nextLevelID);

};

NS_END
