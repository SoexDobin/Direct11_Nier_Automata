#pragma once
#include "StateEm3000.h"


NS_BEGIN(Client)

class CLIENT_DLL StateEm3000_Stomp final : public StateEm3000
{
public:
	explicit StateEm3000_Stomp(const wstring& tag, const Shared<Em3000>& owner);
	~StateEm3000_Stomp() override = default;

private:
	HRESULT Initialize() override;

public:
	Bool StateEnterInvoke() override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void StateExitInvoke() override;

public:
	static Shared<StateEm3000_Stomp> Create(const wstring& tag, const Shared<Em3000>& owner);

};

NS_END

