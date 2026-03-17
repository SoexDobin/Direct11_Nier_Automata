#pragma once
#include "ScriptComponent.h"
#include "State.h"

NS_BEGIN(Engine)

class ENGINE_DLL StateMachine : public ScriptComponent
{
	RTTR_ENABLE(ScriptComponent)
public:
	explicit StateMachine();
	explicit StateMachine(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit StateMachine(const StateMachine& rhs);
	virtual ~StateMachine() override = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* arg) override;
	virtual void Update(Float timeDelta) override;
	virtual void Late_Update(Float timeDelta) override;

public:
	void Add_State(const wstring& stateTag, const Shared<State>& pState);
	void Change_State(const wstring& stateTag);

public:
	virtual Shared<Component> Clone(void* arg = nullptr) override;

private:
	map<wstring, Shared<State>> m_States;
	Shared<State> m_pCurrentState = nullptr;
};

NS_END
