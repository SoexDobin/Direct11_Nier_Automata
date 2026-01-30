#pragma once
#include "Object.h"

NS_BEGIN(Engine)

class ENGINE_DLL Level abstract : public Object
{
public:
	explicit Level(const ComPtr<ID3D11Device>&device, const ComPtr<ID3D11DeviceContext>&context);
	virtual ~Level() override = default;

public:
	virtual HRESULT Initialize(Shared<void> arg) override;
	virtual void On_Destroy() override;
	virtual void On_Disable() override;
	virtual void On_Enable() override;

public:
	virtual void Update_Level(Float timeDelta);
	virtual HRESULT Render_Level();

protected:
	ComPtr<ID3D11Device> m_Device = { nullptr };
	ComPtr<ID3D11DeviceContext> m_Context = { nullptr };

};

NS_END