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
	typedef struct tagLoadingLevelDesc : public LEVEL_DESC
	{
		LEVEL nextLevelID{};
		Bool loadStatic{};
	} LEVEL_LOADING_DESC;
public:
	LevelLoading(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	~LevelLoading() override;

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
	Bool				m_IsLoadStatic{};
	LEVEL				m_NextLevel = {};
	Shared<Loader>		m_Loader = { nullptr };

	Shared<StaticCamera> m_StaticCamera{ nullptr };
	Shared<LoadingBackground> m_Background{nullptr};
	Shared<LoadingPixelPanel> m_PixelPanel{ nullptr };
	Shared<LoadingLogo> m_Logo{ nullptr };

	Shared<LoadingFade>	m_FadeIn{ nullptr };
	Shared<LoadingFade>	m_FadeOut{ nullptr };

public:
	static Shared<LevelLoading> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, LEVEL nextLevelID, Bool loadStatic = false);
	
};

NS_END