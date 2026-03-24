#pragma once
#include "ScriptComponent.h"

NS_BEGIN(Client)

class CLIENT_DLL P10000Input final : public ScriptComponent
{
	RTTR_ENABLE(Component)
public:
	enum class KEY_STATE { NONE, DOWN, PRESSED, UP, MULTI_CLICKED };

public:
	typedef struct tagInputInfo
	{
		KEY_STATE state = KEY_STATE::NONE;
		Float doubleClickTimer{ 999.f };
		Float holdTimer{ 0.f };
	} INPUT_INFO;

public:
	explicit P10000Input();
	explicit P10000Input(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit P10000Input(const P10000Input& rhs);
	virtual ~P10000Input() override = default;

public:
	Bool Is_KeyDown(uByte keyID) const { return m_KeyInfos[keyID].state == KEY_STATE::DOWN; }
	Bool Is_KeyUp(uByte keyID) const { return m_KeyInfos[keyID].state == KEY_STATE::UP; }
	Bool Is_KeyPress(uByte keyID) const { return m_KeyInfos[keyID].state == KEY_STATE::PRESSED || m_KeyInfos[keyID].state == KEY_STATE::MULTI_CLICKED; }
	Bool Is_KeyDoubleClick(uByte keyID) const { return m_KeyInfos[keyID].state == KEY_STATE::MULTI_CLICKED; }
	Bool Is_KeyHold(uByte keyID, Float holdThreshold) const {
		return (m_KeyInfos[keyID].state == KEY_STATE::PRESSED) && (m_KeyInfos[keyID].holdTimer >= holdThreshold);
	}

public:
	Bool Is_MouseDown(Engine::DIMB buttonID) const { return m_MouseInfos[ETOI(buttonID)].state == KEY_STATE::DOWN; }
	Bool Is_MouseUp(Engine::DIMB buttonID) const { return m_MouseInfos[ETOI(buttonID)].state == KEY_STATE::UP; }
	Bool Is_MousePress(Engine::DIMB buttonID) const { return m_MouseInfos[ETOI(buttonID)].state == KEY_STATE::PRESSED || m_MouseInfos[ETOI(buttonID)].state == KEY_STATE::MULTI_CLICKED; }
	Bool Is_MouseDoubleClick(Engine::DIMB buttonID) const { return m_MouseInfos[ETOI(buttonID)].state == KEY_STATE::MULTI_CLICKED; }
	Bool Is_MouseHold(Engine::DIMB buttonID, Float holdThreshold) const {
		return (m_MouseInfos[ETOI(buttonID)].state == KEY_STATE::PRESSED) && (m_MouseInfos[ETOI(buttonID)].holdTimer >= holdThreshold);
	}

public:
	Long Get_MouseMove(DIMM axisType) const { return m_MouseMovement[ETOI(axisType)]; }

public:
	void Update_P10000_InputState(Float timeDelta);

private:
	void Update_P10000_KeyState(Byte rawState, INPUT_INFO& outInfo, Float timeDelta);

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg = nullptr) override;

	void On_Disable() override;
	void On_Enable() override;

private:
	INPUT_INFO m_KeyInfos[256];
	INPUT_INFO m_MouseInfos[ETOI(DIMB::END)];
	LONG m_MouseMovement[ETOI(DIMB::END)];

	Float m_DoubleClickThreshold = 0.25f;

public:
	static Shared<P10000Input> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<Component> Clone(void* arg = nullptr) override;
};

NS_END