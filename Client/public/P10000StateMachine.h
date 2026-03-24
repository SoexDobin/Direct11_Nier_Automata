#pragma once
#include "StateMachine.h"

NS_BEGIN(Client)

class State2B;
class P10000;

class CLIENT_DLL P10000StateMachine final : public StateMachine
{
	RTTR_ENABLE(StateMachine)
public:
	explicit P10000StateMachine();
	explicit P10000StateMachine(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit P10000StateMachine(const P10000StateMachine& rhs);
	~P10000StateMachine() override = default;

public:
	Bool ChangeState(P10000::P10000_STATE state);
	Shared<State2B> Find_2BState(P10000::P10000_STATE state);
	wstring Get_StateTag(P10000::P10000_STATE state);

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg = nullptr) override;
	void On_Destroy() override;
	void On_Disable() override { StateMachine::On_Disable(); }
	void On_Enable() override { StateMachine::On_Enable(); }

public:
	unordered_map<P10000::P10000_STATE, wstring> m_StateTag;

public:
	static Shared<P10000StateMachine> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<Component> Clone(void* arg = nullptr) override;
};

NS_END