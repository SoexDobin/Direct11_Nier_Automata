#pragma once
#include "StateEm3000.h"

NS_BEGIN(Client)

class CLIENT_DLL StateEm3000_Dead final : public StateEm3000
{
public:
	explicit StateEm3000_Dead(const wstring& tag, const Shared<Em3000>& owner);
	~StateEm3000_Dead() override = default;

private:
	HRESULT Initialize() override;

public:
	Bool StateEnterInvoke() override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void StateExitInvoke() override;

private:
	Float m_SinkingSpeed{ 1.5f };
	Float m_TotalSunkY{ 0.f };

	Float m_ExplosionTimer{ 0.f };
	Float m_ExplosionInterval{ 0.4f };

public:
	static Shared<StateEm3000_Dead> Create(const wstring& tag, const Shared<Em3000>& owner);
};

NS_END
