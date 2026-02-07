#include "Game.h"
#include "GameObject.h"
#include "Component.h"
#include "ID_Helper.h"
#include "String_Helper.h"
#include "SpdLogger.h"

template <typename T>
constexpr Shared<T> GameObject::Get_Component() const
{
	uint32 level = Game::GetInstance()->Get_CurrentLevelIndex();

	rttr::type type = rttr::type::get<T>();
	if (type.is_valid() == false) {
		wstring className = Helper::To_wString(typeid(T).name());
		LOG_ERROR(L"Failed To Get Component {} In GameObject {}", className, m_ObjectName);
		MSG_BOX("Failed To Get Component\nCheck Reflection Registration");
		return nullptr;
	}

	if (Shared<T> prototype = Game::GetInstance()->Find_Prototype<T>(level))
	{
		uint32 typeID = prototype->Get_TypeID();
		if (m_Components.contains(typeID)) {
			return static_pointer_cast<T>(m_Components.at(typeID));
		}
	}

	wstring className = Helper::To_wString(type.get_name().to_string());
	LOG_WARN(L"Component {} Not Found In GameObject {}", className, m_ObjectName);
	return nullptr;
}

inline Shared<Component> GameObject::Get_Component(ObjectID typeID) const
{
	if (m_Components.contains(typeID)) {
		return m_Components.at(typeID);
	}

	const wstring& className = Game::GetInstance()->Get_PrototypeName(typeID);
	LOG_WARN(L"Component {} Not Found In GameObject {}", className, m_ObjectName);
	return nullptr;
}

inline Shared<Component> GameObject::Get_Component(const wstring& className) const
{
	uint32 typeID = Game::GetInstance()->Get_PrototypeID(className);

	if (m_Components.contains(typeID)) {
		return m_Components.at(typeID);
	}

	LOG_WARN(L"Component {} Not Found In GameObject {}", className, m_ObjectName);
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

	Game::GetInstance()->

	if (Shared<T> prototype = Game::GetInstance()->Find_Prototype<T>(level))
	{
		uint32 typeID = prototype->Get_TypeID();
		if (m_Components.contains(typeID)) {
			return static_pointer_cast<T>(m_Components.at(typeID));
		}
	}

	

	// 컴포넌트 등록
	m_Components.emplace(typeID, newComponent);

	wstring className = Helper::To_wString(type.get_name().to_string());
	LOG_INFO(L"Component {} Added Successfully", className);

	return newComponent;
}

Shared<Component> GameObject::Get_Component(uint32 id, Bool Is_ObjectID) const
{
}

inline Shared<Component> GameObject::Add_Component(uint32 typeID, const Shared<void>& arg)
{
	if (m_Components.contains(typeID)) {
		LOG_ERROR(L"Component ID:{} Already Exists In GameObject", typeID);
		MSG_BOX("Component Already Exists");
		return nullptr;
	}

	// TypeID로 컴포넌트 생성 (Factory 패턴 필요)
	// 현재는 구현 불가 - Prototype Manager를 통해야 함
	LOG_ERROR(L"Add_Component By TypeID Not Implemented Yet");
	MSG_BOX("Use Template Version Or PrototypeManager");
	return nullptr;
}

inline Shared<Component> GameObject::Add_Component(const wstring& className, const Shared<void>& arg)
{
	uint32 typeID = ID_Helper::Get_TypeID(className);
	if (typeID == 0) {
		LOG_ERROR(L"Component {} - Invalid TypeID", className);
		MSG_BOX("Invalid Component ClassName");
		return nullptr;
	}

	return Add_Component(typeID, arg);
}
