#include "ContainerObject.h"

#include "Game.h"
#include "PartObject.h"
#include "SpdLogger.h"

ContainerObject::ContainerObject() : GameObject{} {}
ContainerObject::ContainerObject(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: GameObject{ device, context } {}
ContainerObject::ContainerObject(const ContainerObject& rhs)
	: GameObject{ rhs } {}
HRESULT ContainerObject::Initialize_Prototype()
{
	return GameObject::Initialize_Prototype();
}

HRESULT ContainerObject::Initialize(void* arg)
{
	return GameObject::Initialize(arg);
}

void ContainerObject::On_Destroy()
{
	for (auto& [tag, partObj] : m_PartObjects)
	{
		if (partObj && !partObj->Is_Destroy())
		{
			partObj->On_Destroy();
			Destroy(partObj);
		}
	}
	m_PartObjects.clear();

	GameObject::On_Destroy();
}

void ContainerObject::On_Enable()
{
	for (auto& [tag, partObj] : m_PartObjects)
	{
		if (partObj)
		{
			partObj->Set_Active(false);
		}
	}

	GameObject::On_Enable();
}

void ContainerObject::On_Disable()
{
	for (auto& [tag, partObj] : m_PartObjects)
	{
		if (partObj)
		{
			partObj->Set_Active(true);
		}
	}

	GameObject::On_Disable();
}

HRESULT ContainerObject::Add_PartObject(uint32 prototypeLevIndex, const wstring& prototypeTag, const wstring& partTag, void* arg)
{
	if (nullptr != Find_PartObject(partTag))
	{
		LOG_ERROR(L"Already Exist PartObject {}, Tag {}", partTag, prototypeTag);
		return E_FAIL;
	}

	auto partObject = static_pointer_cast<PartObject>(GAME_INSTANCE->Instantiate<PartObject>(prototypeTag, prototypeLevIndex, arg));
	if (nullptr == partObject)
	{
		LOG_ERROR(L"Failed to Add PartObject {} / {}", prototypeTag, partTag);
		return E_FAIL;
	}
		

	m_PartObjects.emplace(partTag, partObject);
	return S_OK;
}

Shared<PartObject> ContainerObject::Find_PartObject(const wstring& partTag)
{
	if (m_PartObjects.contains(partTag))
	{
		return m_PartObjects[partTag];
	}

	return nullptr;
}
