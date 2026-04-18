#pragma once
#include "WorldObject.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CLIENT_DLL CityOfRuinCentral final : public WorldObject
{
	RTTR_ENABLE(WorldObject)
public:
	explicit CityOfRuinCentral() = default;
	explicit CityOfRuinCentral(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit CityOfRuinCentral(const CityOfRuinCentral& rhs);
	~CityOfRuinCentral() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;
	void On_Enable() override;
	void On_Disable() override;

public:
	HRESULT Render() override;
	void Submit_RenderGroup() override;

public:
	static Shared<CityOfRuinCentral> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
};

NS_END