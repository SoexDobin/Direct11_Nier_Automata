#pragma once
#include "WorldObject.h"

NS_BEGIN(Engine)
class Shader;
class Model;
class Navigation;
NS_END

NS_BEGIN(Client)

class CLIENT_DLL CityOfRuinBuilding final : public WorldObject
{
	RTTR_ENABLE(WorldObject)
public:
	explicit CityOfRuinBuilding() = default;
	explicit CityOfRuinBuilding(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit CityOfRuinBuilding(const CityOfRuinBuilding& rhs);
	~CityOfRuinBuilding() override = default;

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
	static Shared<CityOfRuinBuilding> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
};

NS_END