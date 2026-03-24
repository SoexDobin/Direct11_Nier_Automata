#pragma once
#include "State.h"

NS_BEGIN(Client)
	class P10000;
NS_END

NS_BEGIN(Client)

class CLIENT_DLL State2B abstract : public State
{
	RTTR_ENABLE(State)
public:
	explicit State2B(const wstring& tag, const Shared<P10000>& owner);
	virtual ~State2B() override;

protected:
	Weak<P10000> m_Owner{};
};

NS_END
