#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)

class InputDevice final {
public:
  InputDevice();
  ~InputDevice();

public:
    Byte Get_DIKeyState(uByte keyID) const { return m_ByKeyStates[keyID]; }
    Byte Get_DIMouseState(DIMB buttonID) const {
      return m_MouseState.rgbButtons[ETOI(buttonID)];
    }
    Long Get_DIMouseMove(DIMM mouseState) {
      return *(reinterpret_cast<const Long *>(&m_MouseState) +
               static_cast<uint32>(mouseState));
    }

public:
    HRESULT Initialize(HWND hWnd, HINSTANCE hInst);
    void Update();

private:
  LPDIRECTINPUT8 m_InputSDK = {nullptr};
  LPDIRECTINPUTDEVICE8 m_Keyboard = {nullptr};
  LPDIRECTINPUTDEVICE8 m_Mouse = {nullptr};

  Byte m_ByKeyStates[256] = {};
  DIMOUSESTATE m_MouseState = {};

public:
  static Unique<InputDevice> Create(HWND hWnd, HINSTANCE hInst);
};

NS_END