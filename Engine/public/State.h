#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)

class ENGINE_DLL State abstract
{
public:
	virtual ~State() = default;

public:
	virtual void Enter() PURE;
	virtual void Update(Float timeDelta) PURE;
	virtual void Late_Update(Float timeDelta) PURE;
	virtual void Exit() PURE;
};

NS_END
