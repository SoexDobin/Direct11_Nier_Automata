#pragma once
#include "PartObject.h"

NS_BEGIN(Engine)
class Model;
NS_END

NS_BEGIN(Client)

class StateMachine;

class CLIENT_DLL P10000Body final : public PartObject
{
	RTTR_ENABLE(PartObject)
public:
	explicit P10000Body();
	explicit P10000Body(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit P10000Body(const P10000Body& rhs);
	~P10000Body() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;



public:
	void Priority_Update(Float timeDelta) override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void Fixed_Update(Float fixedDelta) override;
	HRESULT Render() override;
	void Submit_RenderGroup() override;

private:
	HRESULT Bind_ShaderResources();
	HRESULT Ready_Components();
	HRESULT Ready_States();

private:
	Shared<StateMachine> m_StateMachine{nullptr};
	Shared<Shader> m_Shader{ nullptr };
	Shared<Model> m_Model{ nullptr };

public:
	static Shared<P10000Body> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
};

NS_END
