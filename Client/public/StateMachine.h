#pragma once
#include "State.h"
#include "ScriptComponent.h"

NS_BEGIN(Client)

class CLIENT_DLL StateMachine : public ScriptComponent
{
	RTTR_ENABLE()
public:
	explicit StateMachine();
	explicit StateMachine(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit StateMachine(const StateMachine& rhs);
	~StateMachine() override = default;

public:
	Add_State(const wstring& stateTag, const Shared<State>&);

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg = nullptr) override;
	void On_Destroy() override;
	void On_Disable() override;
	void On_Enable() override;



public:
	static Shared<StateMachine> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<Component> Clone(void* arg = nullptr) override;
};

NS_END