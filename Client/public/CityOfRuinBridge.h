#pragma once
#include "WorldObject.h"

NS_BEGIN(Engine)
	class Shader;
	class Model;
	class Navigation;
NS_END

NS_BEGIN(Client)

class CLIENT_DLL CityOfRuinBridge final : public WorldObject
{
	RTTR_ENABLE(WorldObject)
public:
	explicit CityOfRuinBridge();
	explicit CityOfRuinBridge(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit CityOfRuinBridge(const CityOfRuinBridge& rhs);
	~CityOfRuinBridge() override = default;

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
	static Shared<CityOfRuinBridge> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
};

NS_END