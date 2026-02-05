#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL ScriptComponent abstract : public Component
{
public:
	ScriptComponent();
	virtual ~ScriptComponent() override = default;

public:
	COMPONENT_TYPE Get_ComponentType() const override { return COMPONENT_TYPE::SCRIPT; }

};

NS_END