#pragma once
#include "WorldUIObject.h"

NS_BEGIN(Engine)
class Shader;
class Texture;
class VIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class Entity;

class CLIENT_DLL HpBarWorldUI final : public WorldUIObject
{
	RTTR_ENABLE(WorldUIObject)
public:
	typedef struct tagHpBarWorldUIDesc : public WORLD_UI_DESC
	{
		Shared<Entity> target{nullptr};
		Vector3 worldOffset{0.f, 0.f, 0.f};
	} HP_BAR_WORLD_UI_DESC;

public:
	explicit HpBarWorldUI();
	explicit HpBarWorldUI(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit HpBarWorldUI(const HpBarWorldUI& rhs);
	virtual ~HpBarWorldUI() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;

public:
	void Late_Update(Float timeDelta) override;
	void Priority_Update(Float timeDelta) override;
	void Update(Float timeDelta) override;

	void Submit_RenderGroup() override;
	HRESULT Render() override;

private:
	HRESULT Ready_Components();

private:
	Weak<Entity> m_Target{};
	Vector3 m_WorldOffset = { 0.f, 0.f, 0.f };

	Shared<Shader> m_Shader{ nullptr };
	Shared<Texture> m_Texture{ nullptr };
	Shared<VIBuffer_Rect> m_BufferRect{ nullptr };

public:
	static Shared<HpBarWorldUI> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
	
};

NS_END
