#pragma once

#include "Game.h"
#include "LevelManager.h"
#include "ObjectManager.h"
#include "PrototypeManager.h"
#include "SpdLogger.h"

#include "GameObject.h"
#include "Component.h"


NS_BEGIN(Engine)

template <typename T>
constexpr Shared<const T> Game::Find_Prototype(PROTOTYPE prototype, uint32 levIndex) const
{
	uint32 level = levIndex == MAXINT32 ? m_LevelManager->Get_CurrentLevelIndex() : levIndex;

    rttr::type type = rttr::type::get<T>();
    if (type.is_valid() == false) {
        LOG_ERROR(L"Failed To Find Prototype {} At Level{}", Helper::To_wString(typeid(T).name()), level);
    	MSG_BOX("Failed To Find Prototype \n Check Reflection");
		return nullptr;
    }

    const wstring& className = Helper::To_wString(type.get_name().to_string());

    if (Shared<Object> object 
        = m_PrototypeManager->Find_Prototype(prototype, level, className))
    {
		return static_pointer_cast<T>(object);
    }

    LOG_WARN(L"Failed To Find Prototype {} At Level{}", className, level);
	MSG_BOX("Failed To Find GameObject Prototype");
	return nullptr;
}

inline Shared<const Object> Game::Find_Prototype(PROTOTYPE prototype, uint32 typeID, uint32 levIndex) const
{
	uint32 level = levIndex == MAXINT32 ? m_LevelManager->Get_CurrentLevelIndex() : levIndex;

	if (Shared<Object> object 
        = m_PrototypeManager->Find_Prototype(prototype, level, typeID))
    {
		return object;
	}

    LOG_WARN(L"Failed To Find Prototype TypeID : {} At Level{}", typeID, level);
    MSG_BOX("Failed To Find Prototype");
    return nullptr;
}

inline Shared<const Object> Game::Find_Prototype(PROTOTYPE prototype, const wstring &className, uint32 levIndex) const
{
	uint32 level = levIndex == MAXINT32 ? m_LevelManager->Get_CurrentLevelIndex() : levIndex;

	if (Shared<Object> object 
        = m_PrototypeManager->Find_Prototype(prototype, level, className))
    {
		return object;
	}

    LOG_WARN(L"Failed To Find Prototype {} At Level{}", className, level);
    MSG_BOX("Failed To Find Prototype");
    return nullptr;
}

template <typename T>
constexpr Shared<T> Game::Instantiate(void* arg) const
{
	uint32 level = m_LevelManager->Get_CurrentLevelIndex();

	rttr::type type = rttr::type::get<T>();
	if (type.is_valid() == false) {
        LOG_ERROR(L"Failed To Find Prototype {} At Level{}", Helper::To_wString(typeid(T).name()), level);
        MSG_BOX("Failed To Find GameObject Prototype \n I Think Reflection Failed");
        return nullptr;
	}

    PROTOTYPE typeTag = {};
    if constexpr (is_base_of_v<GameObject, T>)
        typeTag = PROTOTYPE::GAMEOBJECT;
    else
        typeTag = PROTOTYPE::COMPONENT;

    const wstring& className = Helper::To_wString(type.get_name().to_string());

    if (auto instance = Instantiate_Internal(typeTag, level, className))
    {
		return static_pointer_cast<T>(instance);
    }

    LOG_WARN(L"Failed To Clone GameObject {} At Level{}", className, level);
    MSG_BOX("Failed To Clone GameObject");
    return nullptr;
}

template <typename T>
constexpr Shared<T> Game::Instantiate(uint32 typeID, void* arg) const
{
    uint32 level = m_LevelManager->Get_CurrentLevelIndex();

    PROTOTYPE typeTag = {};
    if constexpr (is_base_of_v<GameObject, T>)
        typeTag = PROTOTYPE::GAMEOBJECT;
    else
        typeTag = PROTOTYPE::COMPONENT;

    if (auto instance = Instantiate_Internal(typeTag, level, typeID, arg))
    {
		return static_pointer_cast<T>(instance);
    }

    LOG_WARN(L"Failed To Clone Object TypeID : {} At Level{}", typeID, level);
    MSG_BOX("Failed To Clone Object");
    return nullptr;
}

template <typename T>
constexpr Shared<T> Game::Instantiate(const wstring &className, void* arg) const
{
    uint32 level = m_LevelManager->Get_CurrentLevelIndex();

    PROTOTYPE typeTag = {};
    if constexpr (is_base_of_v<GameObject, T>)
        typeTag = PROTOTYPE::GAMEOBJECT;
    else
        typeTag = PROTOTYPE::COMPONENT;

    if (auto instance = Instantiate_Internal(typeTag, level, className, arg))
    {
        return static_pointer_cast<T>(instance);
    }

    LOG_WARN(L"Failed To Clone Object {} At Level{}", className, level);
    MSG_BOX("Failed To Create Object");
    return nullptr;
}

Shared<Object> Game::Instantiate_Internal(PROTOTYPE prototype, uint32 levIndex, uint32 typeID, void* arg) const
{
    auto prototypeInstance = m_PrototypeManager->Find_Prototype(prototype, levIndex, typeID);
    if (!prototypeInstance) return nullptr;

    if (prototypeInstance->Get_Prototype() == PROTOTYPE::GAMEOBJECT)
    {
        auto gameObject = static_pointer_cast<GameObject>(prototypeInstance)->Clone(arg);
        m_ObjectManager->Add_GameObject(gameObject);

        return gameObject;
    }
    else if (prototypeInstance->Get_Prototype() == PROTOTYPE::COMPONENT)
    {
        return static_pointer_cast<Component>(prototypeInstance)->Clone(arg);
    }

    LOG_CRITICAL(L"Prototype Miss Match In Instantiate Internal");
    MSG_BOX("Prototype Miss Match In Instantiate Internal");
    return nullptr;
}

Shared<Object> Game::Instantiate_Internal(PROTOTYPE prototype, uint32 levIndex, const wstring& className, void* arg) const
{
    auto prototypeInstance = m_PrototypeManager->Find_Prototype(prototype, levIndex, className);
    if (!prototypeInstance) return nullptr;

    if (prototypeInstance->Get_Prototype() == PROTOTYPE::GAMEOBJECT)
    {
        auto gameObject = static_pointer_cast<GameObject>(prototypeInstance)->Clone(arg);
        m_ObjectManager->Add_GameObject(gameObject);

        return gameObject;
    }
    else if (prototypeInstance->Get_Prototype() == PROTOTYPE::COMPONENT)
    {
        return static_pointer_cast<Component>(prototypeInstance)->Clone(arg);
    }

    LOG_CRITICAL(L"Prototype Miss Match In Instantiate Internal");
    MSG_BOX("Prototype Miss Match In Instantiate Internal");
    return nullptr;
}

NS_END

