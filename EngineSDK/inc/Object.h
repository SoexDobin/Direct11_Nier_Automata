#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)

class ENGINE_DLL Object abstract : public enable_shared_from_this<Object>
{
public:
	Object();
	virtual ~Object();

public:
	uint32 Get_TypeID() const			{ return m_ObjectDesc.typeID; }
	uint32 Get_ObjectID() const			{ return m_ObjectDesc.uniqueID; }
	const wstring& Get_Name() const		{ return m_ObjectName; }

public:
	virtual HRESULT Initialize_Prototype() { return S_OK; }
	virtual HRESULT Initialize(Shared<void> arg) { return S_OK; }
	virtual void On_Destroy() { return; }
	virtual void On_Disable() { return; }
	virtual void On_Enable() { return; }

	virtual void Set_Active(Bool isActive);
	Bool Is_Active() const;

public:
	static void Destroy(Shared<Object> object);

protected:
	Bool m_IsDestroy		= { false }; 
	Bool m_IsActive			= { true };
	ID_DESC m_ObjectDesc	= {};
	wstring m_ObjectName	= {};

};

NS_END