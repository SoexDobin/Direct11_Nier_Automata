#pragma once
#include "Game.h"
#include "GameObject.h"
#include "Component.h"
#include "ID_Helper.h"
#include "String_Helper.h"
#include "SpdLogger.h"


NS_BEGIN(Engine)

template <typename T>
constexpr Shared<T> GameObject::Get_Component()
{
	uint32 level = Game::GetInstance()->Get_CurrentLevelIndex();

	rttr::type type = rttr::type::get<T>();
	if (type.is_valid() == false) {
		wstring className = Helper::To_wString(typeid(T).name());
		LOG_ERROR(L"Failed To Get Component {} In GameObject {}", className, m_ObjectName);
		MSG_BOX("Failed To Get Component\nCheck Reflection Registration");
		return nullptr;
	}

	if (Shared<const Component> prototype = static_pointer_cast<const Component>
		(Game::GetInstance()->Find_Prototype<T>(PROTOTYPE::COMPONENT, level)))
	{
		if (prototype->Get_ComponentType() == COMPONENT_TYPE::SCRIPT
			&& m_Scripts.contains(prototype->Get_ObjectID()))
		{
			return static_pointer_cast<T>(m_Scripts[prototype->Get_ObjectID()]);
		}

		if (m_Components.contains(ETOI(prototype->Get_ComponentType())))
		{
			return static_pointer_cast<T>(m_Components[ETOI(prototype->Get_ComponentType())]);
		}
	}

	wstring className = Helper::To_wString(type.get_name().to_string());
	LOG_WARN(L"Component {} Not Found In GameObject {}", className, m_ObjectName);
	return nullptr;
}

Shared<Component> GameObject::Get_Component(uint32 objectID) 
{
	if (m_Scripts.contains(objectID))
	{
		return m_Scripts[objectID];
	}

	for (const auto& component : m_Components)
	{
		if (component.second->Get_ObjectID() == objectID)
			return component.second;
	}

	return nullptr;
}

template <typename T>
constexpr Shared<T> GameObject::Add_Component(const Shared<void>& arg)
{
	uint32 level = Game::GetInstance()->Get_CurrentLevelIndex();

	rttr::type type = rttr::type::get<T>();
	if (type.is_valid() == false) {
		wstring className = Helper::To_wString(typeid(T).name());
		LOG_ERROR(L"Failed To Get Component {} In GameObject {}", className, m_ObjectName);
		MSG_BOX("Failed To Get Component\nCheck Reflection Registration");
		return nullptr;
	}

	if (Shared<Component> instance = Game::GetInstance()->Instantiate<T>(arg))
	{
		if (instance->Get_ComponentType() == COMPONENT_TYPE::SCRIPT
			&& !m_Scripts.contains(instance->Get_ObjectID()))
		{
			m_Scripts.emplace(instance->Get_ObjectID(), instance);
		}
		else if (!m_Components.contains(ETOI(instance->Get_ComponentType())))
		{
			m_Components.emplace(ETOI(instance->Get_ComponentType()), instance);
		}

		instance->Set_Owner(shared_from_this());
		return instance;
	}

	wstring className = Helper::To_wString(type.get_name().to_string());
	LOG_ERROR(L"Failed To Add Component {} In GameObject {}", className, m_ObjectName);
	MSG_BOX("Failed To Add Component\nCheck Reflection Registration");
	return nullptr;
}

NS_END
