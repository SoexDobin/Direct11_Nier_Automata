#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)

class ENGINE_DLL Object abstract
{
public:
	typedef struct tagGuidDesc
	{
		uint32 m_typeID = {};
		uint32 m_objectID = {};
	} ID_DESC, ObjectID;

public:
	Object();
	virtual ~Object();

public:
	uint32 Get_TypeID() const			{ return m_ObjectDesc.m_typeID; }
	uint32 Get_ObjectID() const			{ return m_ObjectDesc.m_objectID; }
	const wstring& Get_Name() const		{ return m_ObjectName; }

public:
	virtual HRESULT Initialize_Prototype() { return S_OK; }
	virtual HRESULT Initialize(const Shared<void>& arg) { return S_OK; }

	Bool Is_Destroy() const { return m_IsDestroy; }
	virtual void On_Destroy() { return; }

	virtual void On_Disable() { return; }
	virtual void On_Enable() { return; }

	virtual void Set_Active(Bool isActive);
	Bool Is_Active() const;

public:
	virtual PROTOTYPE Get_Prototype() const PURE;
	static void Destroy(const Shared<Object>& object);

protected:
	Bool m_IsDestroy		= { false }; 
	Bool m_IsActive			= { true };
	ID_DESC m_ObjectDesc	= {};
	wstring m_ObjectName	= {};

};

NS_END