#pragma once
#include "UIObject.h"

NS_BEGIN(Engine)
class Shader;
class Texture;
class VIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class Entity;

class CLIENT_DLL Pl0000HpBar final : public UIObject
{
	RTTR_ENABLE(UIObject)
public:
	typedef struct tagPl0000HpBarDesc : public UI_DESC
	{
		Shared<Entity> target{nullptr};
	} PL0000_HP_BAR_DESC;

public:
	explicit Pl0000HpBar();
	explicit Pl0000HpBar(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Pl0000HpBar(const Pl0000HpBar& rhs);
	virtual ~Pl0000HpBar() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;

public:
	void Priority_Update(Float timeDelta) override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;

	HRESULT Render() override;

private:
	HRESULT Ready_Components();

private:
	Weak<Entity> m_Target{};
	Float m_HpRatio{1.f};

	Shared<Shader> m_Shader{ nullptr };
	Shared<Texture> m_Texture{ nullptr };
	Shared<VIBuffer_Rect> m_BufferRect{ nullptr };

public:
	static Shared<Pl0000HpBar> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
};

NS_END
