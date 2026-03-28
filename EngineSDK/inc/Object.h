#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)

class ENGINE_DLL Object abstract {
	RTTR_ENABLE()
public:
	typedef struct tagObjectDesc{} OBJECT_DESC;
public:
	Object();
	virtual ~Object();

public:
	uint32 Get_TypeID() const { return m_DescID.m_typeID; }
	uint32 Get_ObjectID() const { return m_DescID.m_objectID; }
	uint32 Get_InstanceID() const { return m_DescID.m_instanceID; }
	void Set_ObjectID(uint32 objectID) { m_DescID.m_objectID = objectID; }
	const wstring &Get_Name() const { return m_ObjectName; }
	void Set_Name(const wstring& name) { m_ObjectName = name; }

public:
	virtual HRESULT Initialize_Prototype();
	HRESULT Initialize_Prototype(const wstring& prototypeTag);
	virtual HRESULT Initialize(void *arg) { return S_OK; }
	virtual HRESULT Begin() { return S_OK; }

	Bool Is_Destroy() const { return m_IsDestroy; }
	virtual void On_Destroy() { return; }

	virtual void On_Disable() { return; }
	virtual void On_Enable() { return; }

	virtual void Set_Active(Bool isActive);
	Bool Is_Active() const;

	OBJECT_DESC* Get_ObjectDesc() const { return m_ObjectDesc; }

public:
	virtual PROTOTYPE Get_Prototype() const PURE;
	static void Destroy(const Shared<Object> &object);

protected:
	OBJECT_DESC* m_ObjectDesc{};
	Bool m_IsDestroy = {false};
	Bool m_IsActive = {true};
	ID_DESC m_DescID = {};
	wstring m_ObjectName = {};
};

NS_END