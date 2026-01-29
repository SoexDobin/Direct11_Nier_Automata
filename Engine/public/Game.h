#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)

class GraphicDevice;
class TimeManager;

class ENGINE_DLL Game
{
	DECLARE_SINGLETON(Game)

protected:
	explicit Game() {}
	~Game() = default;

public:
	HRESULT Initialize_Engine(const ENGINE_DESC& engineDesc,
							  _Out_ ComPtr<ID3D11Device>& device,
							  _Out_ ComPtr<ID3D11DeviceContext>& context);
	void Update_Engine() const;
	HRESULT Draw() const;
	void Clear_Resource();

private:
	Unique<GraphicDevice> m_GraphicDevice = { nullptr };
	Unique<TimeManager> m_TimeManager = { nullptr };
};

NS_END
