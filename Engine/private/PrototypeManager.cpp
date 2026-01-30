#include "PrototypeManager.h"
#include "GameObject.h"
#include "Component.h"

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

HRESULT PrototypeManager::Add_Prototype(uint32 levIndex, const wstring& key, const Shared<Object>& prototype)
{
	if (levIndex >= m_LevelCount || prototype == nullptr)
		return E_FAIL;

	// TODO : Add 할때 리플렉션으로 클래스 이름 가져와서 이름으로 키 사용

	return S_OK;
}

Shared<Object> PrototypeManager::Clone_Prototype(PROTOTYPE prototype, 
												 uint32 levIndex, 
												 const wstring& key,
												 Shared<void> arg)
{
	Shared<Object> searchedPrototype = Find_Prototype(levIndex, key);
	if (searchedPrototype == nullptr)
		return nullptr;

	Shared<Object> instance = { nullptr };

	if (PROTOTYPE::GAMEOBJECT == prototype)
	{
		instance = static_pointer_cast<GameObject>(searchedPrototype)->Clone(arg);
	}
	else if (PROTOTYPE::COMPONENT == prototype)
	{
		instance = static_pointer_cast<Component>(searchedPrototype)->Clone(arg);
	}

	if (instance == nullptr)
	{
		MSG_BOX("Failed To Clone Prototype");
		return nullptr;
	}

	return instance;
}	

HRESULT PrototypeManager::Clear_Prototypes(uint32 levIndex)
{
	if (levIndex >= m_LevelCount)
		return E_FAIL;

	m_Prototypes[levIndex].clear();

	return S_OK;
}

Shared<Object> PrototypeManager::Find_Prototype(uint32 levIndex, const wstring& key)
{
	if (levIndex >= m_LevelCount)
		return nullptr;

	auto it = m_Prototypes[levIndex].find(key);
	if (it == m_Prototypes[levIndex].end())
		return nullptr;

	return it->second;
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
