#pragma once
#include "Object.h"

NS_BEGIN(Engine)

class Manager abstract : public Object
{
	NO_COPY(Manager)
public:
	Manager() = default;
	virtual ~Manager() override = default;
};

NS_END