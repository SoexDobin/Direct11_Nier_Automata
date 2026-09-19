#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
	class Light;
NS_END

NS_BEGIN(Client)

/* Scene-placed directional light. The light shines along the Transform's look axis, so it is
   aimed by rotating the object; Color and Ambient are reflected for the Inspector and the scene. */
class CLIENT_DLL DirectionalLight final : public GameObject
{
public:
	explicit DirectionalLight();
	explicit DirectionalLight(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit DirectionalLight(const DirectionalLight& rhs);
	virtual ~DirectionalLight() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;
	void On_Enable() override;
	void On_Disable() override;

public:
	void Priority_Update(Float timeDelta) override {}
	void Update(Float timeDelta) override {}
	void Late_Update(Float timeDelta) override {}
	void Fixed_Update(Float fixedDelta) override {}
	HRESULT Render() override { return S_OK; }
	void Submit_RenderGroup() override;

public:
	const Color& Get_Color() const { return m_Color; }
	void Set_Color(const Color& color) { m_Color = color; }
	const Color& Get_Ambient() const { return m_Ambient; }
	void Set_Ambient(const Color& ambient) { m_Ambient = ambient; }

private:
	Shared<Light> m_Light{ nullptr };
	Color m_Color{ 1.f, 1.f, 1.f, 1.f };
	Color m_Ambient{ 0.2f, 0.2f, 0.2f, 1.f };

public:
	static Shared<DirectionalLight> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
};

NS_END
