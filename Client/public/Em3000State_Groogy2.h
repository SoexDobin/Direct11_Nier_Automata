#pragma once
#include "StateEm3000.h"


NS_BEGIN(Client)

class CLIENT_DLL Em3000State_Groogy2 final : public StateEm3000
{
public:
	explicit Em3000State_Groogy2(const wstring& tag, const Shared<Em3000>& owner);
	~Em3000State_Groogy2() override = default;

private:
	HRESULT Initialize() override;

public:
	Bool StateEnterInvoke() override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void StateExitInvoke() override;

private:
	Float m_GroggyDelta{0.f};

public:
	static Shared<Em3000State_Groogy2> Create(const wstring& tag, const Shared<Em3000>& owner);

};

NS_END

