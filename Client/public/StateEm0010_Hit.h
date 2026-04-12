#pragma once
#include "StateEm0010.h"

NS_BEGIN(Client)

class CLIENT_DLL StateEm0010_Hit final : public StateEm0010
{
public:
	explicit StateEm0010_Hit(const wstring& tag, const Shared<Em0010>& owner);
	~StateEm0010_Hit() override;

private:
	HRESULT Initialize() override;

public:
	Bool StateEnterInvoke() override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void StateExitInvoke() override;

private:
	Bool Is_BackAttack();

private:
	const Float n_ReChaseDistance = 8.f;

public:
	static Shared<StateEm0010_Hit> Create(const wstring& tag, const Shared<Em0010>& owner);
};

NS_END
