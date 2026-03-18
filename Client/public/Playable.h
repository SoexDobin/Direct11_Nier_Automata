#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CLIENT_DLL Playable abstract : public GameObject
{
	RTTR_ENABLE(GameObject)
public:
	explicit Playable();
	explicit Playable(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Playable(const Playable& rhs);
	virtual ~Playable() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;
	void On_Enable() override;
	void On_Disable() override;

public:
	void Priority_Update(Float timeDelta) override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void Fixed_Update(Float fixedDelta) override;
	HRESULT Render() override;
	void Submit_RenderGroup() override;

protected:

public:
	Shared<GameObject> Clone(void* arg) override PURE;
};

NS_END