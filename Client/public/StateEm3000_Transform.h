#pragma once
#include "StateEm3000.h"


NS_BEGIN(Client)

class CLIENT_DLL StateEm3000_Transform final : public StateEm3000
{
public:
	explicit StateEm3000_Transform(const wstring& tag, const Shared<Em3000>& owner);
	~StateEm3000_Transform() override = default;

private:
	HRESULT Initialize() override;

public:
	Bool StateEnterInvoke() override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void StateExitInvoke() override;

public:
	static Shared<StateEm3000_Transform> Create(const wstring& tag, const Shared<Em3000>& owner);

};

NS_END

