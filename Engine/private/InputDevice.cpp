#include "InputDevice.h"

InputDevice::InputDevice() { ZeroMemory(m_ByKeyStates, sizeof(m_ByKeyStates)); }
InputDevice::~InputDevice() {
  m_InputSDK->Release();
  m_Mouse->Release();
  m_Keyboard->Release();
}

HRESULT InputDevice::Initialize(HWND hWnd, HINSTANCE hInst) {
  if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8,
                                reinterpret_cast<void **>(&m_InputSDK),
                                nullptr)))
    return E_FAIL;

  if (FAILED(m_InputSDK->CreateDevice(GUID_SysKeyboard, &m_Keyboard, nullptr)))
    return E_FAIL;
  m_Keyboard->SetDataFormat(&c_dfDIKeyboard);
  m_Keyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
  m_Keyboard->Acquire();

  if (FAILED(m_InputSDK->CreateDevice(GUID_SysMouse, &m_Mouse, nullptr)))
    return E_FAIL;
  m_Mouse->SetDataFormat(&c_dfDIMouse);
  m_Mouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
  m_Mouse->Acquire();

  return S_OK;
}

void InputDevice::Update() {
	if (FAILED(m_Keyboard->GetDeviceState(256, m_ByKeyStates))) {
		m_Keyboard->Acquire();
	}
	if (FAILED(m_Mouse->GetDeviceState(sizeof(m_MouseState), &m_MouseState))) {
		m_Mouse->Acquire();
	}
}

Unique<InputDevice> InputDevice::Create(HWND hWnd, HINSTANCE hInst) {
  auto inputDevice = make_unique<InputDevice>();

  if (FAILED(inputDevice->Initialize(hWnd, hInst))) {
    MSG_BOX("Failed To Create : InputDevice");
    return nullptr;
  }

  return inputDevice;
}
