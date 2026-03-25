#include "pch.h"
#include "P10000Input.h"

#include <Game.h>

P10000Input::P10000Input() : ScriptComponent{} {}
P10000Input::P10000Input(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context)
	: ScriptComponent{device, context}
{
	ZeroMemory(&m_KeyInfos, sizeof(INPUT_INFO) * KEY_MAX);
	ZeroMemory(&m_MouseInfos, sizeof(INPUT_INFO) * ETOI(DIMB::END));
	ZeroMemory(&m_MouseMovement, sizeof(LONG) * ETOI(DIMB::END));
}
P10000Input::P10000Input(const P10000Input &rhs)
	: ScriptComponent{rhs} {}

HRESULT P10000Input::Initialize_Prototype()
{
	return ScriptComponent::Initialize_Prototype();
}

HRESULT P10000Input::Initialize(void *arg)
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

// ===================================================================
// 매 프레임 갱신
// ===================================================================
void P10000Input::Update_P10000_InputState(Float timeDelta)
{
	if (!Is_Active() || !GAME_INSTANCE->Get_InputEnabled()) return;

	// --- 키보드 ---
	for (uint32 i = 0; i < KEY_MAX; ++i)
	{
		Byte rawState = GAME_INSTANCE->Get_DIKeyState(static_cast<uByte>(i));
		Update_P10000_KeyState(rawState, m_KeyInfos[i], timeDelta);
	}

	// --- 마우스 버튼 + 콤보 큐 ---
	for (uint32 i = 0; i < ETOI(DIMB::END); ++i)
	{
		DIMB buttonID = static_cast<DIMB>(i);
		Byte rawState = GAME_INSTANCE->Get_DIMouseState(buttonID);
		Update_P10000_KeyState(rawState, m_MouseInfos[i], timeDelta);

		// DOWN 이 발생한 프레임에만 큐에 적재
		if (m_MouseInfos[i].state == KEY_STATE::DOWN)
		{
			m_MouseComboQueue.push_back(buttonID);
			m_ComboTimer = 0.f;
		}
	}

	// --- 콤보 타임아웃 ---
	if (!m_MouseComboQueue.empty())
	{
		m_ComboTimer += timeDelta;
		if (m_ComboTimer > m_ComboTimeout)
		{
			m_MouseComboQueue.clear();
		}
	}

	// --- 마우스 이동 ---
	for (uint32 i = 0; i < ETOI(DIMM::END); ++i)
	{
		DIMM axisType = static_cast<DIMM>(i);
		m_MouseMovement[i] = GAME_INSTANCE->Get_DIMouseMove(axisType);
	}
}

// ===================================================================
// 키 상태 전이 (DOWN → PRESSED → UP → NONE)
// ===================================================================
void P10000Input::Update_P10000_KeyState(Byte rawState, INPUT_INFO &outInfo, Float timeDelta)
{
	Bool isPressed = (rawState & 0x80) != 0;

	outInfo.doubleClickTimer += timeDelta;

	if (isPressed)
	{
		if (outInfo.state == KEY_STATE::NONE || outInfo.state == KEY_STATE::UP)
		{
			// 이전 UP → 다시 눌림: 더블클릭 판정
			if (outInfo.doubleClickTimer <= m_DoubleClickThreshold)
			{
				outInfo.state = KEY_STATE::MULTI_CLICKED;
				outInfo.isMultiClickHold = true;
			}
			else
			{
				outInfo.state = KEY_STATE::DOWN;
				outInfo.isMultiClickHold = false;
			}
			outInfo.doubleClickTimer = 0.f;
			outInfo.holdTimer = 0.f;
		}
		else // DOWN, PRESSED, MULTI_CLICKED → 유지 중
		{
			outInfo.state = KEY_STATE::PRESSED;
			outInfo.holdTimer += timeDelta;
		}
	}
	else
	{
		if (outInfo.state == KEY_STATE::DOWN || outInfo.state == KEY_STATE::PRESSED || outInfo.state == KEY_STATE::MULTI_CLICKED)
		{
			outInfo.state = KEY_STATE::UP;
			outInfo.prevHoldTimer = outInfo.holdTimer;
			outInfo.holdTimer = 0.f;
		}
		else
		{
			outInfo.state = KEY_STATE::NONE;
		}
	}
}

// ===================================================================
// WASD 통합 제어 (Bool)
// ===================================================================
Bool P10000Input::Is_WASD_Down() const
{
	return Is_KeyDown(DIKEYBOARD_W) || Is_KeyDown(DIKEYBOARD_A) ||
		   Is_KeyDown(DIKEYBOARD_S) || Is_KeyDown(DIKEYBOARD_D);
}

Bool P10000Input::Is_WASD_Press() const
{
	return Is_KeyPress(DIKEYBOARD_W) || Is_KeyPress(DIKEYBOARD_A) ||
		   Is_KeyPress(DIKEYBOARD_S) || Is_KeyPress(DIKEYBOARD_D);
}

Bool P10000Input::Is_WASD_DoubleClick() const
{
	return Is_KeyMultiClick(DIKEYBOARD_W) || Is_KeyMultiClick(DIKEYBOARD_A) ||
		   Is_KeyMultiClick(DIKEYBOARD_S) || Is_KeyMultiClick(DIKEYBOARD_D);
}

Bool P10000Input::Is_WASD_Hold(Float holdThreshold) const
{
	return Is_KeyHold(DIKEYBOARD_W, holdThreshold) || Is_KeyHold(DIKEYBOARD_A, holdThreshold) ||
		   Is_KeyHold(DIKEYBOARD_S, holdThreshold) || Is_KeyHold(DIKEYBOARD_D, holdThreshold);
}

Bool P10000Input::Is_WASD_Diagonal() const
{
	Bool w = Is_KeyPress(DIKEYBOARD_W);
	Bool s = Is_KeyPress(DIKEYBOARD_S);
	Bool a = Is_KeyPress(DIKEYBOARD_A);
	Bool d = Is_KeyPress(DIKEYBOARD_D);

	// 상반된 축 동시 입력은 대각선이 아님
	if (w && s) return false;
	if (a && d) return false;

	return (w || s) && (a || d);
}

Bool P10000Input::Is_WA_Press() const
{
	return Is_KeyPress(DIKEYBOARD_W) && Is_KeyPress(DIKEYBOARD_A);
}

Bool P10000Input::Is_WA_DoubleClick() const
{
	if (Is_KeyMultiClick(DIKEYBOARD_W) && Is_KeyPress(DIKEYBOARD_A))
		return true;
	if (Is_KeyPress(DIKEYBOARD_W) && Is_KeyMultiClick(DIKEYBOARD_A))
		return true;
	
	return false;
}

Bool P10000Input::Is_WA_Hold(Float holdThreshold) const
{
	if (Is_KeyHold(DIKEYBOARD_W, holdThreshold) && Is_KeyPress(DIKEYBOARD_A))
		return true;
	if (Is_KeyPress(DIKEYBOARD_W) && Is_KeyHold(DIKEYBOARD_A, holdThreshold))
		return true;

	return false;
}

Bool P10000Input::Is_WA_DoubleClickedHold(Float holdThreshold) const
{
	if (Is_KeyMultiClickHold(DIKEYBOARD_W, holdThreshold) && Is_KeyPress(DIKEYBOARD_A))
		return true;
	if (Is_KeyPress(DIKEYBOARD_W) && Is_KeyMultiClickHold(DIKEYBOARD_A, holdThreshold))
		return true;

	return false;
}

Bool P10000Input::Is_WD_Press() const
{
	return Is_KeyPress(DIKEYBOARD_W) && Is_KeyPress(DIKEYBOARD_D);
}

Bool P10000Input::Is_WD_DoubleClick() const
{
	if (Is_KeyMultiClick(DIKEYBOARD_W) && Is_KeyPress(DIKEYBOARD_D))
		return true;
	if (Is_KeyPress(DIKEYBOARD_W) && Is_KeyMultiClick(DIKEYBOARD_D))
		return true;

	return false;
}

Bool P10000Input::Is_WD_Hold(Float holdThreshold) const
{
	if (Is_KeyHold(DIKEYBOARD_W, holdThreshold) && Is_KeyPress(DIKEYBOARD_D))
		return true;
	if (Is_KeyPress(DIKEYBOARD_W) && Is_KeyHold(DIKEYBOARD_D, holdThreshold))
		return true;

	return false;
}

Bool P10000Input::Is_WD_DoubleClickedHold(Float holdThreshold) const
{
	if (Is_KeyMultiClickHold(DIKEYBOARD_W, holdThreshold) && Is_KeyPress(DIKEYBOARD_D))
		return true;
	if (Is_KeyPress(DIKEYBOARD_W) && Is_KeyMultiClickHold(DIKEYBOARD_D, holdThreshold))
		return true;

	return false;
}

Bool P10000Input::Is_SA_Press() const
{
	return Is_KeyPress(DIKEYBOARD_S) && Is_KeyPress(DIKEYBOARD_A);
}

Bool P10000Input::Is_SA_DoubleClick() const
{
	if (Is_KeyMultiClick(DIKEYBOARD_S) && Is_KeyPress(DIKEYBOARD_A))
		return true;
	if (Is_KeyPress(DIKEYBOARD_S) && Is_KeyMultiClick(DIKEYBOARD_A))
		return true;

	return false;
}

Bool P10000Input::Is_SA_Hold(Float holdThreshold) const
{
	if (Is_KeyHold(DIKEYBOARD_S, holdThreshold) && Is_KeyPress(DIKEYBOARD_A))
		return true;
	if (Is_KeyPress(DIKEYBOARD_S) && Is_KeyHold(DIKEYBOARD_A, holdThreshold))
		return true;

	return false;
}

Bool P10000Input::Is_SA_DoubleClickedHold(Float holdThreshold) const
{
	if (Is_KeyMultiClickHold(DIKEYBOARD_S, holdThreshold) && Is_KeyPress(DIKEYBOARD_A))
		return true;
	if (Is_KeyPress(DIKEYBOARD_S) && Is_KeyMultiClickHold(DIKEYBOARD_A, holdThreshold))
		return true;

	return false;
}

Bool P10000Input::Is_SD_Press() const
{
	return Is_KeyPress(DIKEYBOARD_S) && Is_KeyPress(DIKEYBOARD_D);
}

Bool P10000Input::Is_SD_DoubleClick() const
{
	if (Is_KeyMultiClick(DIKEYBOARD_S) && Is_KeyPress(DIKEYBOARD_D))
		return true;
	if (Is_KeyPress(DIKEYBOARD_S) && Is_KeyMultiClick(DIKEYBOARD_D))
		return true;

	return false;
}

Bool P10000Input::Is_SD_Hold(Float holdThreshold) const
{
	if (Is_KeyHold(DIKEYBOARD_S, holdThreshold) && Is_KeyPress(DIKEYBOARD_D))
		return true;
	if (Is_KeyPress(DIKEYBOARD_S) && Is_KeyHold(DIKEYBOARD_D, holdThreshold))
		return true;

	return false;
}

Bool P10000Input::Is_SD_DoubleClickedHold(Float holdThreshold) const
{
	if (Is_KeyMultiClickHold(DIKEYBOARD_S, holdThreshold) && Is_KeyPress(DIKEYBOARD_D))
		return true;
	if (Is_KeyPress(DIKEYBOARD_S) && Is_KeyMultiClickHold(DIKEYBOARD_D, holdThreshold))
		return true;

	return false;
}

// ===================================================================
// 마우스 콤보 큐 판별
// ===================================================================
Bool P10000Input::Is_MouseComboMatch(const vector<Engine::DIMB> &pattern) const
{
	if (m_MouseComboQueue.size() != pattern.size())
		return false;

	for (size_t i = 0; i < pattern.size(); ++i)
	{
		if (m_MouseComboQueue[i] != pattern[i])
			return false;
	}
	return true;
}

Bool P10000Input::Is_MouseComboStartsWith(const vector<Engine::DIMB> &pattern) const
{
	if (m_MouseComboQueue.size() < pattern.size())
		return false;

	for (size_t i = 0; i < pattern.size(); ++i)
	{
		if (m_MouseComboQueue[i] != pattern[i])
			return false;
	}
	return true;
}

Engine::DIMB P10000Input::Get_MouseComboLast() const
{
	if (m_MouseComboQueue.empty())
		return Engine::DIMB::END;

	return m_MouseComboQueue.back();
}

// ===================================================================
// 팩토리
// ===================================================================
Shared<P10000Input> P10000Input::Create(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context)
{
	auto prototype = make_shared<P10000Input>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : P10000Input");
		return nullptr;
	}

	return prototype;
}

Shared<Component> P10000Input::Clone(void *arg)
{
	auto instance = make_shared<P10000Input>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : P10000Input");
		return nullptr;
	}

	return instance;
}
