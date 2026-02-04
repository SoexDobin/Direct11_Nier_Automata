#pragma once
#include "Object.h"

NS_BEGIN(Engine)

class EngineManager abstract : public Object
{
	NO_COPY(EngineManager)
public:
	EngineManager() = default;
	virtual ~EngineManager() override = default;

public:
	PROTOTYPE Get_Prototype() const final { return PROTOTYPE::ENGINE_MANAGER; }

private:
	using Object::m_ObjectDesc;
};

NS_END