#include "pch.h"
#include "P10000Input.h"

#include <Game.h>

P10000Input::P10000Input() : ScriptComponent{} {}
P10000Input::P10000Input(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: ScriptComponent{device, context} {}
P10000Input::P10000Input(const P10000Input& rhs)
	: ScriptComponent{rhs} {}

HRESULT P10000Input::Initialize_Prototype()
{
	return ScriptComponent::Initialize_Prototype();
}

HRESULT P10000Input::Initialize(void* arg)
{
	return ScriptComponent::Initialize(arg);
}

void P10000Input::On_Disable()
{
	ScriptComponent::On_Disable();
}

void P10000Input::On_Enable()
{
	ScriptComponent::On_Enable();
}

void P10000Input::Update_P10000_InputState(Float timeDelta)
{
	if (!Is_Active() || !GAME_INSTANCE->Get_InputEnabled()) return;
	
	for (uint32 i = 0; i < KEY_MAX; ++i)
	{
		Byte rawState = GAME_INSTANCE->Get_DIKeyState(static_cast<uByte>(i));
		Update_P10000_KeyState(rawState, m_KeyInfos[i], timeDelta);
	}

	for (uint32 i = 0; i < ETOI(DIMB::END); ++i)
	{
		DIMB buttonID = static_cast<DIMB>(i);
		Byte rawState = GAME_INSTANCE->Get_DIMouseState(buttonID);
		Update_P10000_KeyState(rawState, m_KeyInfos[i], timeDelta);
	}

	for (uint32 i = 0; i < ETOI(DIMM::END); ++i)
	{
		DIMM axisType = static_cast<DIMM>(i);
		m_MouseMovement[i] = GAME_INSTANCE->Get_DIMouseMove(axisType);
	}

}

void P10000Input::Update_P10000_KeyState(Byte rawState, INPUT_INFO& outInfo, Float timeDelta)
{
	Bool isPressed = (rawState & 0x80) != 0;

	outInfo.doubleClickTimer += timeDelta;

	if (isPressed)
	{
		if (outInfo.state == KEY_STATE::NONE || outInfo.state == KEY_STATE::PRESSED || outInfo.state == KEY_STATE::MULTI_CLICKED)
		{
			if (outInfo.doubleClickTimer <= m_DoubleClickThreshold)
			{
				outInfo.state = KEY_STATE::MULTI_CLICKED;
			}
			else
			{
				outInfo.state = KEY_STATE::DOWN;
			}
			outInfo.doubleClickTimer = 0.f;
			outInfo.holdTimer = 0.f;
		}
		else
		{
			outInfo.state = KEY_STATE::PRESSED;
			outInfo.holdTimer += timeDelta;
		}
	}
	else
	{
		if (outInfo.state == KEY_STATE::NONE || outInfo.state == KEY_STATE::PRESSED || outInfo.state == KEY_STATE::MULTI_CLICKED)
		{
			outInfo.state = KEY_STATE::UP;
			outInfo.holdTimer = 0.f;
		}
		else
		{
			outInfo.state = KEY_STATE::NONE;
		}
	}
}


Shared<P10000Input> P10000Input::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<P10000Input>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : P10000Input");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> P10000Input::Clone(void* arg)
{
	auto instance = make_shared<P10000Input>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : P10000Input");
		return nullptr;
	}

	return instance;
}


