#pragma once
#include "StateEm3000.h"


NS_BEGIN(Client)

class CLIENT_DLL StateEm3000_Intro final : public StateEm3000
{
public:
	explicit StateEm3000_Intro(const wstring& tag, const Shared<Em3000>& owner);
	~StateEm3000_Intro() override = default;

private:
	HRESULT Initialize() override;

public:
	Bool StateEnterInvoke() override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void StateExitInvoke() override;

private:
	Bool m_IsIntroStart{false};

public:
	static Shared<StateEm3000_Intro> Create(const wstring& tag, const Shared<Em3000>& owner);

};

NS_END

