#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)

class Registry;

class ENGINE_DLL Object abstract {
	RTTR_ENABLE()
public:
	typedef struct tagObjectDesc{} OBJECT_DESC;
public:
	Object();
	virtual ~Object();

public:
	RuntimeTypeId Get_RuntimeTypeId() const { return m_RuntimeTypeId; }
	RuntimeObjectId Get_RuntimeObjectId() const { return m_RuntimeObjectId; }
	const wstring &Get_Name() const { return m_ObjectName; }
	void Set_Name(const wstring& name) { m_ObjectName = name; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void *arg) { return S_OK; }
	virtual HRESULT Begin() { return S_OK; }

	Bool Is_Destroy() const { return m_IsDestroy; }
	virtual void On_Destroy() { return; }

	virtual void On_Disable() { return; }
	virtual void On_Enable() { return; }

	virtual void Set_Active(Bool isActive);
	Bool Is_Active() const;

	OBJECT_DESC* Get_ObjectDesc() const { return m_ObjectDesc; }

protected:
	void Mark_Destroyed() { m_IsDestroy = true; }
	OBJECT_DESC* m_ObjectDesc{};
	Bool m_IsDestroy = {false};
	Bool m_IsActive = {true};
	RuntimeTypeId m_RuntimeTypeId{};
	RuntimeObjectId m_RuntimeObjectId{};
	wstring m_ObjectName = {};

private:
	friend class Registry;
	friend class ObjectManager;
	friend class GameObject;
	void Assign_ReflectedIdentity(RuntimeTypeId runtimeTypeId, std::string_view registeredName);
};

NS_END
