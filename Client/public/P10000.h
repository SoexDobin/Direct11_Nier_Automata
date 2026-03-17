#pragma once
#include "Playable.h"

NS_BEGIN(Engine)
class StateMachine;
NS_END

class C2B : public Playable
{
	RTTR_ENABLE(Playable)
public:
	explicit C2B();
	explicit C2B(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit C2B(const C2B& rhs);
	virtual ~C2B() override = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* arg) override;
	virtual void Priority_Update(Float timeDelta) override;
	virtual void Update(Float timeDelta) override;
	virtual void Late_Update(Float timeDelta) override;
	virtual void Fixed_Update(Float fixedDelta) override;
	virtual HRESULT Render() override;

public:
	virtual Shared<GameObject> Clone(void* arg) override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	Shared<StateMachine> m_pStateMachine = nullptr;
};
