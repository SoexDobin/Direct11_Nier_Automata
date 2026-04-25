#pragma once

#include "Engine_Define.h"
#include "Game.h"
#include "GameObject.h"
#include "Transform.h"
#include "Client_Define.h"

NS_BEGIN(Client)

class RedDot
{
public:
    static inline void Debug_TeleportPlayer_F1()
    {
#ifdef _DEBUG
        static bool isPressed = false;
        if (GAME_INSTANCE->Get_DIKeyState(DIK_F1) & 0x80)
        {
            if (!isPressed)
            {
                isPressed = true;
                Engine::uint32 levIndex = GAME_INSTANCE->Get_CurrentLevelIndex();
                if (levIndex == static_cast<Engine::uint32>(LEVEL::GAMEPLAY2))
                {
                    Shared<Engine::GameObject> player = GAME_INSTANCE->Find_ObjectByObjectTag(levIndex, L"Pl0000");
                    if (player)
                    {
                        player->Get_Transform()->Set_Position(Vector3(370.f, 25.f, 52.f));
                        player->Get_Transform()->Update_WorldMatrix();
                    }
                }
            }
        }
        else
        {
            isPressed = false;
        }
#endif
    }
};

NS_END
