#pragma once
#include "Level.h"

NS_BEGIN(Client)

class Loader;
class StaticCamera;
class LoadingFade;
class LoadingBackground;
class LoadingLogo;
class LoadingPixelPanel;


class CLIENT_DLL LevelLoading final : public Level
{
public:
	LevelLoading(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	~LevelLoading() override = default;

private:
	HRESULT Initialize_Prototype() override { return S_OK; }
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;

public:
	void Update_Level(Float timeDelta) override;
	void Update_LoadLevel(Float timeDelta) override;
	HRESULT Render_Level() override;

private:
	void Transition_To_NextLevel(Float timeDelta);
	void Ready_LoadingUI();

private:
	LEVEL				m_NextLevel = {};
	Shared<Loader>		m_Loader = { nullptr };

	Shared<StaticCamera> m_StaticCamera{ nullptr };
	Shared<LoadingBackground> m_Background{nullptr};
	Shared<LoadingPixelPanel> m_PixelPanel{ nullptr };
	Shared<LoadingLogo> m_Logo{ nullptr };

	Shared<LoadingFade>	m_FadeIn{ nullptr };
	Shared<LoadingFade>	m_FadeOut{ nullptr };

public:
	static Shared<LevelLoading> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, LEVEL nextLevelID);
	
};

NS_END