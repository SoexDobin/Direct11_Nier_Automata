#pragma once
#include "MonsterStateMachine.h"
#include "Em3000.h"

NS_BEGIN(Client)

class Em3000;
class StateEm3000;

class CLIENT_DLL Em3000StateMachine final : public MonsterStateMachine
{
	RTTR_ENABLE(StateMachine)
public:
	explicit Em3000StateMachine() = default;
	explicit Em3000StateMachine(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Em3000StateMachine(const Em3000StateMachine& rhs);
	~Em3000StateMachine() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg = nullptr) override;
	void On_Destroy() override;
	void On_Disable() override;
	void On_Enable() override;

public:
	Bool Change_State(Em3000::EM3000_STATE state);
	Shared<StateEm3000> Find_Em3000State(Em3000::EM3000_STATE state);
	wstring Get_StateTag(Em3000::EM3000_STATE state);
	Em3000::EM3000_STATE Get_CurEm3000State();



public:
	static Shared<Em3000StateMachine> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<Component> Clone(void* arg = nullptr) override;

};

NS_END