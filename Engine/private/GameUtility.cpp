#include "Game.h"
#include "GraphicDevice.h"
#include "LevelManager.h"
#include "ObjectManager.h"
#include "PrototypeManager.h"
#include "Renderer.h"
#include "TimeManager.h"

#include "Level.h"
#include "Timer.h"

IMPLEMENT_SINGLETON(Game);

template <typename T>
constexpr Shared<const T> Game::Find_Prototype(uint32 levIndex) const 
{
	uint32 level = levIndex == MAXINT32 ? m_LevelManager->Get_CurrentLevelIndex() : levIndex;

    rttr::type type = rttr::type::get<T>();
    if (type.is_valid() == false) {
		// TODO : Warn
    	MSG_BOX("Failed To Find GameObject Prototype \n I Think Reflection Failed");
		return nullptr;
    }

    wstring className = Helper::To_wString(type.get_name().to_string());

    if (Shared<Object> object 
        = m_PrototypeManager->Find_Prototype(PROTOTYPE::GAMEOBJECT, level, className)) 
    {
      return static_pointer_cast<T>(object);
    }

	// TODO : Warn
	MSG_BOX("Failed To Find GameObject Prototype");
	return nullptr;
}

inline Shared<const GameObject> Game::Find_Prototype(uint32 typeID, uint32 levIndex) const
{
	uint32 level = levIndex == MAXINT32 ? m_LevelManager->Get_CurrentLevelIndex() : levIndex;

	if (Shared<Object> object 
        = m_PrototypeManager->Find_Prototype(PROTOTYPE::GAMEOBJECT, level, typeID)) 
    {
		return static_pointer_cast<GameObject>(object);
	}

    // TODO : critical
    MSG_BOX("Failed To Find GameObject Prototype");
    return nullptr;
}

inline Shared<const GameObject> Game::Find_Prototype(const wstring &className, uint32 levIndex) const 
{
	uint32 level = levIndex == MAXINT32 ? m_LevelManager->Get_CurrentLevelIndex() : levIndex;

	if (Shared<Object> object 
        = m_PrototypeManager->Find_Prototype(PROTOTYPE::GAMEOBJECT, level, className)) 
    {
		return static_pointer_cast<GameObject>(object);
	}

    // TODO : critical
    MSG_BOX("Failed To Find GameObject Prototype");
    return nullptr;
}

template <typename T>
constexpr Shared<T> Game::Instantiated(const Shared<void> &arg) const 
{
	uint32 level = m_LevelManager->Get_CurrentLevelIndex();

	rttr::type type = rttr::type::get<T>();
	if (type.is_valid() == false) {
        // TODO : Warn
        MSG_BOX("Failed To Find GameObject Prototype \n I Think Reflection Failed");
        return nullptr;
	}

    wstring className = Helper::To_wString(type.get_name().to_string());

    if (Shared<Object> object 
        = m_PrototypeManager->Clone_Prototype(PROTOTYPE::GAMEOBJECT, level, className)) 
    {
		return static_pointer_cast<T>(object);
    }

    // TODO : Warn
    MSG_BOX("Failed To Find GameObject Prototype");
    return nullptr;
}

inline Shared<GameObject> Game::Instantiated(uint32 typeID, const Shared<void> &arg) const 
{
    uint32 level = m_LevelManager->Get_CurrentLevelIndex();

    if (auto instance 
        = m_PrototypeManager->Clone_Prototype(PROTOTYPE::GAMEOBJECT, level, typeID, arg)) 
    {
		return static_pointer_cast<GameObject>(instance);
    }

    MSG_BOX("Failed To Create GameObject");
    return nullptr;
}

inline Shared<GameObject> Game::Instantiated(const wstring &className, const Shared<void> &arg) const 
{
    uint32 level = m_LevelManager->Get_CurrentLevelIndex();

    if (auto instance 
        = m_PrototypeManager->Clone_Prototype(PROTOTYPE::GAMEOBJECT, level, className, arg)) 
    {
		return static_pointer_cast<GameObject>(instance);
    }

    MSG_BOX("Failed To Create GameObject");
    return nullptr;
}
