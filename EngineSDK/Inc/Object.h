#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)

class ENGINE_DLL Object abstract : public enable_shared_from_this<Object> 
{
public:
    Object();
    virtual ~Object();

public:
	uint32 Get_ObjectID() const { return m_ObjectID; }
	const wstring& Get_Name() const { return m_ObjectName; }

public:
    virtual HRESULT Initialize(void* arg = nullptr) {}
    virtual void OnDestroy() {}
    virtual void OnDisable() {}
    virtual void OnEnable() {}

    virtual void Set_Active(Bool isActive);
    Bool Is_Active() const;

public:
    static void Destroy(Shared<Object> object);
    
    
protected:
    Bool m_IsDestroy        = { false };
    Bool m_IsActive         = { true };
    uint32 m_ObjectID       = {};
    wstring m_ObjectName    = {};
};

NS_END
