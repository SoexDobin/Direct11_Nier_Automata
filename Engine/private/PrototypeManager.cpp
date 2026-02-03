#include "PrototypeManager.h"
#include "GameObject.h"
#include "Component.h"
#include "Type_Helper.h"
#include "String_Helper.h"

HRESULT PrototypeManager::Initialize(Shared<void> arg)
{
	m_LevelCount = *static_pointer_cast<uint32>(arg);
	m_Prototypes.shrink_to_fit();
	m_Prototypes.resize(m_LevelCount);
	return S_OK;
}

void PrototypeManager::On_Destroy()
{
	for (size_t i = 0; i < m_LevelCount; ++i)
	{
		m_Prototypes[i].clear();
	}
}

HRESULT PrototypeManager::Add_Prototype(uint32 levIndex, const Shared<Object>& prototype)
{
	return S_OK;
}

Shared<Object> PrototypeManager::Clone_Prototype(uint32 levIndex, Shared<void> arg)
{

	return nullptr;
}

HRESULT PrototypeManager::Clear_Prototypes(uint32 levIndex)
{
	return S_OK;
}

Shared<Object> PrototypeManager::Find_Prototype(uint32 levIndex, const wstring& name)
{
	return nullptr;
}

HRESULT PrototypeManager::Create_Reflection(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	type type_GameObject = Helper::Get_Type<GameObject>();
	type type_Component = Helper::Get_Type<Component>();

	for (auto& type : type::get_types())
	{
		Bool isDerivedObj = type.is_derived_from(type_GameObject);
		Bool isDerivedCom = type.is_derived_from(type_Component);

		if (!isDerivedObj && !isDerivedCom)
			continue;

		method createMethod = type.get_method("Create");
		if (createMethod.is_valid() == false)
			continue;

		variant result = createMethod.invoke({}, device, context);
		if (result.is_valid() == false)
			continue;

		Shared<Object> prototype = result.get_value<Shared<Object>>();
		if (prototype == nullptr) 
			continue;

		std::wstring typeName = Helper::To_wString(type.get_name().to_string());

		for (size_t i = 0; i < m_LevelCount; ++i)
		{
			m_Prototypes[i].emplace(prototype->Get_TypeID(), prototype);
			m_Types[i].emplace(typeName, prototype->Get_TypeID());
		}

	}
	return S_OK;
}


Unique<PrototypeManager> PrototypeManager::Create(uint32 levCount)
{
	auto prototypeManager = make_unique<PrototypeManager>();

	if (FAILED(prototypeManager->Initialize(Shared<void>(&levCount))))
	{
		MSG_BOX("Failed To Create PrototypeManager");
		return nullptr;
	}

	return prototypeManager;
}
