#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)

class GraphicDevice;
class TimeManager;
class PrototypeManager;

class ENGINE_DLL Game
{
	DECLARE_SINGLETON(Game)

protected:
	explicit Game();
	~Game();

public:
	HRESULT Initialize_Engine(const ENGINE_DESC& engineDesc,
							  _Out_ ComPtr<ID3D11Device>& device,
							  _Out_ ComPtr<ID3D11DeviceContext>& context);
	void	Update_Engine();
	HRESULT Draw();
	void	Clear_Resource(uint32 levIndex);

private:
	Unique<GraphicDevice>		m_GraphicDevice = { nullptr };
	Unique<TimeManager>			m_TimeManager = { nullptr };
	Unique<PrototypeManager>	m_PrototypeManager = { nullptr };
};

NS_END
